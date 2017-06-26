/*
An Efficient Ray/Polygon Intersection
by Didier Badouel
from "Graphics Gems", Academic Press, 1990
https://github.com/erich666/GraphicsGems/blob/3c76e97f8b010cdb974249db976c211e39569257/gems/RayPolygon.c

Gonsolo: Added missing structs and make it a function
*/

#include <stdbool.h>
#include <stdio.h>
#include "RayPolygon.h"

void printVector(Vector v, char* name);
/* Added to make it compile */

bool intersect(struct Polygon *poly, struct Ray *ray, float t, int i1, int i2, int v) {
	float alpha, beta,  u0, u1, u2, v0, v1, v2; //gamma
	bool inter;
	int i;
    Point *V = poly->V;

	/* the value of t is computed.
	 * i1 and i2 come from the polygon description.
	 * V is the vertex table for the polygon and N the
	 * associated normal vectors.
	 */
	ray->P[0] = ray->O[0] + ray->D[0] * t;
	ray->P[1] = ray->O[1] + ray->D[1] * t;
	ray->P[2] = ray->O[2] + ray->D[2] * t;
    if(!v){
        printf("Points at 'intersection': %f, %f, %f\n", ray->P[0], ray->P[1], ray->P[2]);
        printVector(poly->V[0], "V0");
        printVector(poly->V[1], "V1");
        printVector(poly->V[2], "V2");
    }
	u0 = ray->P[i1] - V[0][i1]; v0 = ray->P[i2] - V[0][i2];
	inter = false; i = 2;
	do {
		/* The polygon is viewed as (n-2) triangles. */
		u1 = V[i - 1][i1] - V[0][i1]; v1 = V[i - 1][i2] - V[0][i2];
		u2 = V[i][i1] - V[0][i1]; v2 = V[i][i2] - V[0][i2];

		if (u1 == 0) {
			beta = u0 / u2;
			if ((beta >= 0.) && (beta <= 1.)) {
				alpha = (v0 - beta*v2) / v1;
				inter = ((alpha >= 0.) && (alpha + beta) <= 1.);
			}
		}
		else {
			beta = (v0*u1 - u0*v1) / (v2*u1 - u2*v1);
			if ((beta >= 0.) && (beta <= 1.)) {
				alpha = (u0 - beta*u2) / u1;
				inter = ((alpha >= 0) && ((alpha + beta) <= 1.));
			}
		}
	} while ((!inter) && (++i < poly->n));

	if (inter) {
		/* Storing the intersection point. */
        if(!v){
            printf("Intersection ray->P: %f, %f, %f\n", ray->P[0], ray->P[1], ray->P[2]);
        }
		/* the normal vector can be interpolated now or later. */
		/* if (poly.interpolate) { */
		/* 	gamma = 1 - (alpha + beta); */
		/* 	ray.normal[0] = gamma * N[0][0] + alpha * N[i - 1][0] + */
		/* 		beta * N[i][0]; */
		/* 	ray.normal[1] = gamma * N[0][1] + alpha * N[i - 1][1] + */
		/* 		beta * N[i][1]; */
		/* 	ray.normal[2] = gamma * N[0][2] + alpha * N[i - 1][2] + */
		/* 		beta * N[i][2]; */
		/* } */
	}
	return inter;
}

void printVector(Vector v, char* name){
    /* printf("[%f, %f, %f\n %f, %f, %f\n %f, %f, %f]\n", v[0][0], v[1][0], v[2][0], v[0][1], v[1][1], v[2][1], v[0][2], v[1][2], v[2][2]); */
    printf("%s: [%f, %f, %f]\n",name,  v[0], v[1], v[2]);
}
