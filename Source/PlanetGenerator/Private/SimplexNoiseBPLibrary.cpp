#include "SimplexNoiseBPLibrary.h"

// Simplex Noise implementation based on the public domain code by Stefan Gustavson

// Permutation table
static int32 Perm[512];
static bool bIsInitialized = false;

// Initialize the permutation table with a given seed
static void InitializePermTable(int32 Seed)
{
	FMath::RandInit(Seed);

	// Fill the permutation table
	for (int32 i = 0; i < 256; i++)
	{
		Perm[i] = i;
	}

	// Shuffle the permutation table
	for (int32 i = 255; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		int32 Temp = Perm[i];
		Perm[i] = Perm[j];
		Perm[j] = Temp;
	}

	// Duplicate the permutation table
	for (int32 i = 0; i < 256; i++)
	{
		Perm[i + 256] = Perm[i];
	}

	bIsInitialized = true;
}

// Helper functions
static float Grad(int32 Hash, float X)
{
	int32 h = Hash & 15;
	float Grad = 1.0f + (h & 7);  // Gradient value from 1.0 to 8.0
	if (h & 8) Grad = -Grad;      // Set a random sign
	return Grad * X;              // Multiply the gradient with the distance
}

static float Grad(int32 Hash, float X, float Y)
{
	int32 h = Hash & 7;           // Convert low 3 bits of hash code
	float u = h < 4 ? X : Y;      // into 8 simple gradient directions,
	float v = h < 4 ? Y : X;      // and compute the dot product with (x,y).
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

static float Grad(int32 Hash, float X, float Y, float Z)
{
	int32 h = Hash & 15;          // Convert low 4 bits of hash code
	float u = h < 8 ? X : Y;      // into 12 simple gradient directions,
	float v = h < 4 ? Y : h == 12 || h == 14 ? X : Z; // and compute the dot product with (x,y,z).
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

static float Grad(int32 Hash, float X, float Y, float Z, float W)
{
	int32 h = Hash & 31;          // Convert low 5 bits of hash code
	float u = h < 24 ? X : Y;     // into 32 simple gradient directions,
	float v = h < 16 ? Y : Z;
	float w = h < 8 ? Z : W;
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v) + ((h & 4) ? -w : w);
}

// 1D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise1D(float X)
{
	if (!bIsInitialized)
	{
		InitializePermTable(1337);
	}

	int32 i0 = FMath::FloorToInt(X);
	int32 i1 = i0 + 1;
	float x0 = X - i0;
	float x1 = x0 - 1.0f;

	float n0, n1;

	float t0 = 1.0f - x0 * x0;
	t0 *= t0;
	n0 = t0 * t0 * Grad(Perm[i0 & 0xff], x0);

	float t1 = 1.0f - x1 * x1;
	t1 *= t1;
	n1 = t1 * t1 * Grad(Perm[i1 & 0xff], x1);

	// The maximum value of this noise is 8*(3/4)^4 = 2.53125
	// A factor of 0.395 scales to fit exactly within [-1,1]
	return 0.395f * (n0 + n1);
}

// 2D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise2D(float X, float Y)
{
	if (!bIsInitialized)
	{
		InitializePermTable(1337);
	}

	// Skew the input space to determine which simplex cell we're in
	const float F2 = 0.366025403f; // F2 = (sqrt(3) - 1) / 2
	const float G2 = 0.211324865f; // G2 = (3 - sqrt(3)) / 6

	float s = (X + Y) * F2; // Hairy factor for 2D
	float xs = X + s;
	float ys = Y + s;
	int32 i = FMath::FloorToInt(xs);
	int32 j = FMath::FloorToInt(ys);

	float t = (float)(i + j) * G2;
	float X0 = i - t; // Unskew the cell origin back to (x,y) space
	float Y0 = j - t;
	float x0 = X - X0; // The x,y distances from the cell origin
	float y0 = Y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int32 i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else { i1 = 0; j1 = 1; }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int32 ii = i & 0xff;
	int32 jj = j & 0xff;

	// Calculate the contribution from the three corners
	float n0, n1, n2;

	float t0 = 0.5f - x0 * x0 - y0 * y0;
	if (t0 < 0.0f) n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * Grad(Perm[ii + Perm[jj]], x0, y0);
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;
	if (t1 < 0.0f) n1 = 0.0f;
	else {
		t1 *= t1;
		n1 = t1 * t1 * Grad(Perm[ii + i1 + Perm[jj + j1]], x1, y1);
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;
	if (t2 < 0.0f) n2 = 0.0f;
	else {
		t2 *= t2;
		n2 = t2 * t2 * Grad(Perm[ii + 1 + Perm[jj + 1]], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
}

// 3D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise3D(float X, float Y, float Z)
{
	if (!bIsInitialized)
	{
		InitializePermTable(1337);
	}

	// Skew the input space to determine which simplex cell we're in
	const float F3 = 1.0f / 3.0f;
	const float G3 = 1.0f / 6.0f; // Very nice and simple skew factor for 3D

	float s = (X + Y + Z) * F3; // Very nice and simple skew factor for 3D
	int32 i = FMath::FloorToInt(X + s);
	int32 j = FMath::FloorToInt(Y + s);
	int32 k = FMath::FloorToInt(Z + s);
	float t = (i + j + k) * G3;
	float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
	float Y0 = j - t;
	float Z0 = k - t;
	float x0 = X - X0; // The x,y,z distances from the cell origin
	float y0 = Y - Y0;
	float z0 = Z - Z0;

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	int32 i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
	int32 i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords

	if (x0 >= y0) {
		if (y0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // X Y Z order
		else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; } // X Z Y order
		else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; } // Z X Y order
	}
	else { // x0<y0
		if (y0 < z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; } // Z Y X order
		else if (x0 < z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; } // Y Z X order
		else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // Y X Z order
	}

	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.
	float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
	float y2 = y0 - j2 + 2.0f * G3;
	float z2 = z0 - k2 + 2.0f * G3;
	float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
	float y3 = y0 - 1.0f + 3.0f * G3;
	float z3 = z0 - 1.0f + 3.0f * G3;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int32 ii = i & 0xff;
	int32 jj = j & 0xff;
	int32 kk = k & 0xff;

	// Calculate the contribution from the four corners
	float n0, n1, n2, n3;

	float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 < 0.0f) n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * Grad(Perm[ii + Perm[jj + Perm[kk]]], x0, y0, z0);
	}

	float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 < 0.0f) n1 = 0.0f;
	else {
		t1 *= t1;
		n1 = t1 * t1 * Grad(Perm[ii + i1 + Perm[jj + j1 + Perm[kk + k1]]], x1, y1, z1);
	}

	float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 < 0.0f) n2 = 0.0f;
	else {
		t2 *= t2;
		n2 = t2 * t2 * Grad(Perm[ii + i2 + Perm[jj + j2 + Perm[kk + k2]]], x2, y2, z2);
	}

	float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 < 0.0f) n3 = 0.0f;
	else {
		t3 *= t3;
		n3 = t3 * t3 * Grad(Perm[ii + 1 + Perm[jj + 1 + Perm[kk + 1]]], x3, y3, z3);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0f * (n0 + n1 + n2 + n3); // TODO: The scale factor is preliminary!
}

// 4D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise4D(float X, float Y, float Z, float W)
{
	if (!bIsInitialized)
	{
		InitializePermTable(1337);
	}

	// The skewing and unskewing factors are hairy again for the 4D case
	const float F4 = (FMath::Sqrt(5.0f) - 1.0f) / 4.0f;
	const float G4 = (5.0f - FMath::Sqrt(5.0f)) / 20.0f;

	float s = (X + Y + Z + W) * F4; // Factor for 4D skewing
	int32 i = FMath::FloorToInt(X + s);
	int32 j = FMath::FloorToInt(Y + s);
	int32 k = FMath::FloorToInt(Z + s);
	int32 l = FMath::FloorToInt(W + s);
	float t = (i + j + k + l) * G4; // Factor for 4D unskewing
	float X0 = i - t; // Unskew the cell origin back to (x,y,z,w) space
	float Y0 = j - t;
	float Z0 = k - t;
	float W0 = l - t;

	float x0 = X - X0;  // The x,y,z,w distances from the cell origin
	float y0 = Y - Y0;
	float z0 = Z - Z0;
	float w0 = W - W0;

	// For the 4D case, the simplex is a 4D shape I won't even try to describe.
	// To find out which of the 24 possible simplices we're in, we need to
	// determine the magnitude ordering of x0, y0, z0 and w0.
	// The method below is a reasonable way of finding the ordering of x,y,z,w
	// without any branching.
	int32 c1 = (x0 > y0) ? 32 : 0;
	int32 c2 = (x0 > z0) ? 16 : 0;
	int32 c3 = (y0 > z0) ? 8 : 0;
	int32 c4 = (x0 > w0) ? 4 : 0;
	int32 c5 = (y0 > w0) ? 2 : 0;
	int32 c6 = (z0 > w0) ? 1 : 0;
	int32 c = c1 + c2 + c3 + c4 + c5 + c6;

	int32 i1, j1, k1, l1; // The integer offsets for the second simplex corner
	int32 i2, j2, k2, l2; // The integer offsets for the third simplex corner
	int32 i3, j3, k3, l3; // The integer offsets for the fourth simplex corner

	// simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
	// Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
	// impossible. Only the 24 indices which have non-zero entries make any sense.
	// We use a thresholding to set the coordinates in turn from the largest magnitude.
	// The number 3 in the "simplex" array is at the position of the largest coordinate.
	static const int32 simplex[64][4] = {
		{0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
		{0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
		{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		{1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
		{1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
		{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
		{2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
		{2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
	};

	i1 = simplex[c][0] >= 3 ? 1 : 0;
	j1 = simplex[c][1] >= 3 ? 1 : 0;
	k1 = simplex[c][2] >= 3 ? 1 : 0;
	l1 = simplex[c][3] >= 3 ? 1 : 0;
	// The number 2 in the "simplex" array is at the second largest coordinate.
	i2 = simplex[c][0] >= 2 ? 1 : 0;
	j2 = simplex[c][1] >= 2 ? 1 : 0;
	k2 = simplex[c][2] >= 2 ? 1 : 0;
	l2 = simplex[c][3] >= 2 ? 1 : 0;
	// The number 1 in the "simplex" array is at the second smallest coordinate.
	i3 = simplex[c][0] >= 1 ? 1 : 0;
	j3 = simplex[c][1] >= 1 ? 1 : 0;
	k3 = simplex[c][2] >= 1 ? 1 : 0;
	l3 = simplex[c][3] >= 1 ? 1 : 0;
	// The fifth corner has all coordinate offsets = 1, so no need to look that up.

	// Offsets for second corner in (x,y,z,w) coords
	float x1 = x0 - i1 + G4;
	float y1 = y0 - j1 + G4;
	float z1 = z0 - k1 + G4;
	float w1 = w0 - l1 + G4;
	// Offsets for third corner in (x,y,z,w) coords
	float x2 = x0 - i2 + 2.0f * G4;
	float y2 = y0 - j2 + 2.0f * G4;
	float z2 = z0 - k2 + 2.0f * G4;
	float w2 = w0 - l2 + 2.0f * G4;
	// Offsets for fourth corner in (x,y,z,w) coords
	float x3 = x0 - i3 + 3.0f * G4;
	float y3 = y0 - j3 + 3.0f * G4;
	float z3 = z0 - k3 + 3.0f * G4;
	float w3 = w0 - l3 + 3.0f * G4;
	// Offsets for last corner in (x,y,z,w) coords
	float x4 = x0 - 1.0f + 4.0f * G4;
	float y4 = y0 - 1.0f + 4.0f * G4;
	float z4 = z0 - 1.0f + 4.0f * G4;
	float w4 = w0 - 1.0f + 4.0f * G4;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int32 ii = i & 0xff;
	int32 jj = j & 0xff;
	int32 kk = k & 0xff;
	int32 ll = l & 0xff;

	// Calculate the contribution from the five corners
	float n0, n1, n2, n3, n4;

	float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
	if (t0 < 0.0f) n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * Grad(Perm[ii + Perm[jj + Perm[kk + Perm[ll]]]], x0, y0, z0, w0);
	}

	float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
	if (t1 < 0.0f) n1 = 0.0f;
	else {
		t1 *= t1;
		n1 = t1 * t1 * Grad(Perm[ii + i1 + Perm[jj + j1 + Perm[kk + k1 + Perm[ll + l1]]]], x1, y1, z1, w1);
	}

	float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
	if (t2 < 0.0f) n2 = 0.0f;
	else {
		t2 *= t2;
		n2 = t2 * t2 * Grad(Perm[ii + i2 + Perm[jj + j2 + Perm[kk + k2 + Perm[ll + l2]]]], x2, y2, z2, w2);
	}

	float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
	if (t3 < 0.0f) n3 = 0.0f;
	else {
		t3 *= t3;
		n3 = t3 * t3 * Grad(Perm[ii + i3 + Perm[jj + j3 + Perm[kk + k3 + Perm[ll + l3]]]], x3, y3, z3, w3);
	}

	float t4 = 0.6f - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
	if (t4 < 0.0f) n4 = 0.0f;
	else {
		t4 *= t4;
		n4 = t4 * t4 * Grad(Perm[ii + 1 + Perm[jj + 1 + Perm[kk + 1 + Perm[ll + 1]]]], x4, y4, z4, w4);
	}

	// Sum up and scale the result to cover the range [-1,1]
	return 27.0f * (n0 + n1 + n2 + n3 + n4);
}

// Fractal Brownian Motion (FBM) noise
float USimplexNoiseBPLibrary::SimplexNoiseFBM(float X, float Y, float Z, int32 Octaves, float Persistence, float Lacunarity)
{
	float Total = 0.0f;
	float Frequency = 1.0f;
	float Amplitude = 1.0f;
	float MaxValue = 0.0f;  // Used for normalizing result to 0.0 - 1.0

	for (int32 i = 0; i < Octaves; i++)
	{
		Total += SimplexNoise3D(X * Frequency, Y * Frequency, Z * Frequency) * Amplitude;
		MaxValue += Amplitude;
		Amplitude *= Persistence;
		Frequency *= Lacunarity;
	}

	return Total / MaxValue;
}

// Set the seed for the noise
void USimplexNoiseBPLibrary::SetNoiseSeed(int32 NewSeed)
{
	InitializePermTable(NewSeed);
}
