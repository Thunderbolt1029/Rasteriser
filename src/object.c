#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "object.h"
#include "list.h"

Object* LoadObjFile(char* objFileName) {
    FILE *fptr = fopen(objFileName, "r");
    if (fptr == NULL) {
        fclose(fptr);
        return NULL;
    } 

    int triCount = 0;
    Object *obj = malloc(sizeof(Object));
    obj->transform = (Transform){ 0, 0, 0, 0, 0, 0 };
    obj->texture = NULL;
    obj->colour = (Pixel){255,255,255};

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Element* vertexList = CreateList();
    Element* normalList = CreateList();
    Element* textureList = CreateList();
    Element* triangleList = CreateList();

    struct triStackElement { 
        float3 vertex; 
        float3 normal; 
        float2 texture; 
    };
    int triSP = 0;
    struct triStackElement triStack[3];

    while ((read = getline(&line, &len, fptr)) != -1) {
        char *next;
        switch (line[0])
        {
        case 'v':
            switch (line[1])
            {
            case ' ':
                float3 *vertex = malloc(sizeof(float3));
                vertex->x = strtof(line+2, &next);
                vertex->y = strtof(next, &next);
                vertex->z = strtof(next, NULL);
                ListAdd(vertexList, vertex);
                break;
            case 'n':
                float3 *normal = malloc(sizeof(float3));
                normal->x = strtof(line+3, &next);
                normal->y = strtof(next, &next);
                normal->z = strtof(next, NULL);
                ListAdd(normalList, normal);
                break;
            case 't':
                float2 *texture = malloc(sizeof(float2));
                texture->x = strtof(line+3, &next);
                texture->y = strtof(next, NULL);
                ListAdd(textureList, texture);
                break;
                
            default:
                break;
            }
            break;
        case 'f':
            int v, t, n;
            next = line + 1;
            for(;;)
            {
                next = ReadVertex(next+1, &v, &t, &n);

                triStack[triSP++] = (struct triStackElement){
                    *(float3*)ListGet(vertexList, v), 
                    *(float3*)ListGet(normalList, n), 
                    t == -1 ? ZERO2 : *(float2*)ListGet(textureList, t)
                };

                if (triSP == 3) {
                    struct triStackElement a = triStack[0];
                    struct triStackElement b = triStack[1];
                    struct triStackElement c = triStack[2];
                    
                    triStack[1] = triStack[2];
                    triSP = 2;
                    
                    triCount++;
                    Tri* tri = malloc(sizeof(Tri));
                    *tri = (Tri){
                        {a.vertex, b.vertex, c.vertex},
                        {a.normal, b.normal, c.normal},
                        {a.texture, b.texture, c.texture}
                    };
                    ListAdd(triangleList, tri);
                }

                if (*next == '\n')
                    break;
            }
            triSP = 0;
            break;
            
        default:
            break;
        }
    }

    obj->triCount = triCount;
    obj->tris = malloc(sizeof(Tri) * triCount);

    Tri* tri;
    int i = 0;
    while ((tri = ListGet(triangleList, 0)) != NULL) {
        obj->tris[i++] = *tri;
        ListRemove(triangleList,tri);
    }

    fclose(fptr);
    return obj;
}

char* ReadVertex(char* s, int* v, int* t, int* n) {
    char *next;
    *v = strtol(s, &next, 10) - 1;
    if (next[1] == '/') { 
        *t = -1;
        *n = strtol(next+2, &next, 10) - 1;
    }
    else {
        *t = strtol(next+1, &next, 10) - 1;
        *n = strtol(next+1, &next, 10) - 1;
    }
    return next;
}

void FreeObject(Object* object) {
    free(object->tris);
    free(object);
}
