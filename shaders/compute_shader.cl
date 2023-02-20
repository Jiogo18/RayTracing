#define ChunkSize 8
#define M_2PI (2 * M_PI)

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

// // Test if there is an intersection in the world
// __kernel void interactions(__global bool *result,
//                             const __global double3 *camera_pos, const __global double2 *camera_rot, const __global long2 *screen_size,
//                             const __global int *chunk_count, const __global double3 *chunk_pos,
//                             const __global int *solids_count_per_chunk, const __global double3 *solids_pMin, const __global double3 *solids_pMax)
// {
//     int index = get_global_id(0);
//     result[index] = index % 2 == 0;

// //	int index = get_global_id(0);
// //	float3 acc = (float3)(0, 0, 0);
// //
// //	for (int i = 0; i < *interaction_rate * get_global_size(0); i++) {
// //		if (i != index) {
// //			float3 vector = convert_3(positions[i]) - convert_3(positions[index]);
// //			acc += (normalize(vector) / (norm_2(vector) + *smoothing_length)) / *interaction_rate;
// //		}
// //	}
// //
// //	acc += (*black_hole_mass * normalize(-convert_3(positions[index]))) / (norm_2(convert_3(positions[index])) + *smoothing_length);
// //	accelerations[index] = convert_4(acc);
// }

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
