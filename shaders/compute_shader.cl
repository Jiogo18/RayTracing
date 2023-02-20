#define CHUNK_SIZE 8
#define M_2PI (2 * M_PI)

// Note : double3 is interpreted as double4.
// Which means that a double3* is actually a double4*.
// If you have an array for double3 : {{1,2,3},{4,5,6}}
// Then it is actually {{1,2,3,4},{5,6,0,0}} in opencl
// It is better to use double* and use the index as a multiple of 4 : {1,2,3,4,5,6}[index*4 + 0] for x of index's point

// {re, im} -> {r, angle}
double2 exp_to_cart(double2 e)
{
    return (double2)(e.x * cos(e.y), e.x * sin(e.y));
}

// {r, angle} -> {re, im}
double2 cart_to_exp(double2 p)
{
    double r = distance(p, (double2){0, 0});
    double angle = atan(p.y / p.x);
    if (p.x < 0) angle += M_PI;
    return (double2)(r, angle);
}

double pourcentage_rayon(double angle, double cible)
{
    double delta = angle - cible;
    delta = delta - floor(delta / M_2PI) * M_2PI;
    if (delta > M_PI) delta = M_2PI - delta;
    return 1 - delta / M_PI;
}

#define viewDistance (100.0);
#define angleHMax (120.0 * M_PI / 180.0) // de 156° à 172° max pour un Humain (1 oeil)
#define angleVMax (100.0 * M_PI / 180.0) // 180° = inf, 0° = 0
#define xMax (tan(angleHMax / 2))
#define yMax (tan(angleVMax / 2))
#define pppH (1)
#define pppV (1)
#define ppp (pppV * pppH)
#define gamma (2)

// Addition de deux angles
double2 getChildRot(double2 rot1, double2 rot2)
{
    const double cx1 = cos(rot1.x), sx1 = sin(rot1.x),
                 cz1 = cos(rot1.y), sz1 = sin(rot1.y),
                 cx2 = cos(rot2.x), sx2 = sin(rot2.x),
                 cz2 = cos(rot2.y), sz2 = sin(rot2.y);
    const double x1 = cz2 * cx2, y1 = cz2 * sx2, z1 = sz2;           // z1==sz2
    const double x2 = x1 * cz1 - z1 * sz1, z2 = x1 * sz1 + z1 * cz1; // y2==y1
    const double x3 = x2 * cx1 - y1 * sx1, y3 = x2 * sx1 + y1 * cx1; // z3==z2

    double rX = atan(y3 / x3);
    if (x3 < 0) {   // sinon on ne voit que tout ce qui est en x3>=0 (rotation de 180° du reste)
        rX += M_PI; // atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        // pas besoin de différentier y car on est à k360° de diff
    }
    return (double2){rX, asin(z2)};
}

// Fill the image with a color wheel
// Called with a 2D grid of work items
__kernel void color_wheel(__global uchar *imageRGBA)
{
    int width = get_global_size(0);
    int height = get_global_size(1);
    int x = get_global_id(0);
    int y = get_global_id(1);
    int index = x + y * width;

    double2 pCart = (double2)(x - width / 2.0f, y - height / 2.0f);
    double2 pExp = cart_to_exp(pCart);
    double dr = pourcentage_rayon(pExp.y, 0 * M_PI / 180);
    double dg = pourcentage_rayon(pExp.y, 120 * M_PI / 180);
    double db = pourcentage_rayon(pExp.y, 240 * M_PI / 180);
    double teinte = pExp.x / 40;

    uchar r = min((int)(dr * teinte * 255), 255);
    uchar g = min((int)(dg * teinte * 255), 255);
    uchar b = min((int)(db * teinte * 255), 255);

    // result[x + y * width] = (int3)(r, g, b);
    imageRGBA[index * 4 + 0] = r;
    imageRGBA[index * 4 + 1] = g;
    imageRGBA[index * 4 + 2] = b;
    imageRGBA[index * 4 + 3] = 255;
}

double2 calc_ray_angle(double2 camera_rot, int2 pixel_pos, int2 screen_size)
{
    // return (double2)(camera_rot.x + pixel_pos.x / screen_size.x * camera_rot.y, camera_rot.y + pixel_pos.y / screen_size.y * camera_rot.x);

    double xPos = (2.0 * pixel_pos.x / screen_size.x - 1) * xMax;
    double xzPos = sqrt(xPos * xPos + 1);

    double angleH = atan(xPos);
    double yPos = (-1 + 2.0 * pixel_pos.y / screen_size.y) * yMax;
    double angleV = atan(yPos / xzPos);

    return getChildRot(camera_rot, (double2){angleH, angleV});
}

void ray_tracing_test_angle(__global uchar *imageRGBA, const int index, const double2 ray_angle)
{
    // horizontalement : rouge
    // verticalement : vert
    imageRGBA[index * 4 + 0] = min((int)(ray_angle.x * 255 / M_2PI), 255);
    imageRGBA[index * 4 + 1] = min((int)(ray_angle.y * 255 / M_2PI), 255);
    imageRGBA[index * 4 + 2] = 0;
}

void ray_tracing_test_vector(__global uchar *imageRGBA, const int index, const double3 ray_vector)
{
    // axe x : rouge
    // axe y : vert
    // axe z : bleu
    imageRGBA[index * 4 + 0] = clamp((int)(ray_vector.x * 255 + 128), 0, 255);
    imageRGBA[index * 4 + 1] = clamp((int)(ray_vector.y * 255 + 128), 0, 255);
    imageRGBA[index * 4 + 2] = clamp((int)(ray_vector.z * 255 + 128), 0, 255);
}

double3 rot_to_vector(double2 angle)
{
    double cx = cos(angle.x), sx = sin(angle.x),
           cz = cos(angle.y), sz = sin(angle.y);
    return (double3){cx * cz, sx * cz, sz};
}

double3 get_next_point(double3 point, double2 angle)
{
    double3 vector = rot_to_vector(angle);
    return (double3){point.x + vector.x, point.y + vector.y, point.z + vector.z};
}

double3 extract_double3_from_array(const __global double *array, const int index)
{
    return (double3){array[index * 3 + 0], array[index * 3 + 1], array[index * 3 + 2]};
}

/*
// Get the intersection of a ray with an object
double get_intersection(const double3 ray_pos, const double3 ray_vector, const double3 pMin, const double3 pMax)
{
    // // Voir #Intersection
    // Size3D ABs(pA, pB);
    // if (paralleleDroite(ABs)) return Point3DCancelable();
    // doubli t = -(a * pA.x() + b * pA.y() + c * pA.z() + d)
    //            / (a * ABs.dX() + b * ABs.dY() + c * ABs.dZ());
    // if (t < 0) // derriere
    //     return Point3DCancelable();
    // return Point3DCancelable(pA + ABs * t);

    const double3 ABs = ray_vector;
    // paralleleDroite :
    // return (a * vect.dX() + b * vect.dY() + c * vect.dZ()) == 0;
    if ((ABs.x == 0 && ABs.y == 0 && ABs.z == 0))
        return -1;
}
*/
bool pass_through(const double3 pA, const double3 vDir, const double3 pMin, const double3 pMax)
{
    // est ce que la droite peut atteindre le block ?
    // voir # Calculs pour HRect3D::containsLine
    // https://github.com/Jiogo18/RayTracing/blob/master/tests/equations.txt

    const double t1X = (pMin.x - pA.x) / vDir.x;
    const double t2X = (pMax.x - pA.x) / vDir.x;
    const double t1Y = (pMin.y - pA.y) / vDir.y;
    const double t2Y = (pMax.y - pA.y) / vDir.y;
    const double t1Z = (pMin.z - pA.z) / vDir.z;
    const double t2Z = (pMax.z - pA.z) / vDir.z;

    const double tMinX = min(t1X, t2X);
    const double tMinY = min(t1Y, t2Y);
    const double tMinZ = min(t1Z, t2Z);
    const double tMin = max(tMinX, max(tMinY, tMinZ));

    const double tMaxX = max(t1X, t2X);
    const double tMaxY = max(t1Y, t2Y);
    const double tMaxZ = max(t1Z, t2Z);
    const double tMax = min(tMaxX, min(tMaxY, tMaxZ));
    return tMin <= tMax; // si la droite ne passe pas dans le rectangle
    // alors la droite rentre et sort du rectangle
    // t \in [tMin;tMax] => avec un peu de chance (ça dépend de la forme) le tMin est celui d'une face
}

// Get the first intersection of a ray with a solid
// Return the index of the chunk and the index of the solid
int2 get_first_intersection(const double3 ray_pos, const double3 ray_vector,
                            const __global int *chunk_count, const __global double3 *chunk_pos, const __global ulong2 *chunk_data,
                            const __global double *solids_pMin, const __global double *solids_pMax)
{
    int solid_count, solid_index;

    int best_chunk_index = -1;
    int best_solid_index = -1;
    double best_distance = 0;

    for (unsigned int chunk_index = 0; chunk_index < *chunk_count; chunk_index++) {

        if (!pass_through(ray_pos, ray_vector, chunk_pos[chunk_index], chunk_pos[chunk_index] + (double3){CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE}))
            continue;

        solid_count = chunk_data[chunk_index].x;
        solid_index = chunk_data[chunk_index].y;

        for (unsigned int i = 0; i < solid_count; i++, solid_index++) {
            double3 pMin = extract_double3_from_array(solids_pMin, solid_index);
            double3 pMax = extract_double3_from_array(solids_pMax, solid_index);
            if (!pass_through(ray_pos, ray_vector, pMin, pMax))
                continue;

            return (int2){chunk_index, solid_index};
            // double distance = get_intersection(ray_angle, ray_pos, solids_pMin[solid_index], solids_pMax[solid_index]);
            // We will assume they are all cubes for now
        }
    }

    return (int2){best_chunk_index, best_solid_index};
}

// Fill the image using RayTracing
__kernel void ray_tracing(__global uchar *imageRGBA,
                          const __global double3 *camera_pos, const __global double2 *camera_rot,
                          const __global int *chunk_count, const __global double3 *chunk_pos, const __global ulong2 *chunk_data,
                          const __global double *solids_pMin, const __global double *solids_pMax)
{
    // double3 *camera_pos  // double[3]
    // double2 *camera_rot  // double[2]
    // int *chunk_count     // int
    // double3 *chunk_pos   // double[chunkCount][3]
    // ulong2 *chunk_data   // ulong[chunkCount][2] (count, offset)
    // double3 *solids_pMin // double[chunkCount][solidsCount][3]
    // double3 *solids_pMax // double[chunkCount][solidsCount][3]

    int width = get_global_size(0);
    int height = get_global_size(1);
    int x = get_global_id(0);
    int y = get_global_id(1);
    int index = x + y * width;

    double2 ray_angle = calc_ray_angle(*camera_rot, (int2)(x, y), (int2)(width, height));
    double3 ray_vector = rot_to_vector(ray_angle);
    // ray_tracing_test_angle(imageRGBA, index, ray_angle);
    // ray_tracing_test_vector(imageRGBA, index, ray_vector);

    int2 chunk_solid_index = get_first_intersection(*camera_pos, ray_vector, chunk_count, chunk_pos, chunk_data, solids_pMin, solids_pMax);

    if (chunk_solid_index.x == -1 || chunk_solid_index.y == -1) {
        imageRGBA[index * 4 + 0] = 0;
        imageRGBA[index * 4 + 1] = 0;
        imageRGBA[index * 4 + 2] = 0;
        imageRGBA[index * 4 + 3] = 255;
        return;
    } else {
        int code = chunk_solid_index.y % 7 + 1;
        imageRGBA[index * 4 + 0] = code & 0b001 ? 255 : 0;
        imageRGBA[index * 4 + 1] = code & 0b010 ? 255 : 0;
        imageRGBA[index * 4 + 2] = code & 0b100 ? 255 : 0;
        // imageRGBA[index * 4 + 2] = clamp(chunk_solid_index.y + 128, 0, 255);
        imageRGBA[index * 4 + 3] = 255;
        // imageRGBA[index * 4 + 0] = min(chunk_solid_index.y * 32, 255);
        // imageRGBA[index * 4 + 1] = 0;
        // imageRGBA[index * 4 + 2] = 255;
        // imageRGBA[index * 4 + 3] = 255;
        return;
    }
}