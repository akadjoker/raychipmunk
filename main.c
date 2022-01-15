/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "chipmunk.h"

#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
//#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define SAFE_FREE(p)             do { if(p) { free(p); (p) = NULL; } } while(0)
#define SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)



Color newColor(float r, float g, float b, float a)
{
  Color c;
  c.r=r*255.0f;
  c.g=g*255.0f;
  c.b=b*255.0f;
  c.a=a*255.0f;
  return c;

}
static Vector2* cpVertArray2ccpArrayN(const cpVect* cpVertArray, unsigned int count)
{
    if (count == 0) return NULL;
    Vector2* pPoints = malloc( count* sizeof(Vector2)) ;

    for (unsigned int i = 0; i < count; ++i)
    {
        pPoints[i].x = cpVertArray[i].x;
        pPoints[i].y = cpVertArray[i].y;
    }
    return pPoints;
}

static Color ColorForBody(cpBody *body)
{
	if (cpBodyIsRogue(body) || cpBodyIsSleeping(body))
    {
		return newColor(0.5f, 0.5f, 0.5f ,0.5f);
	}
    else if (body->CP_PRIVATE(node).idleTime > body->CP_PRIVATE(space)->sleepTimeThreshold)
    {
		return newColor(0.33f, 0.33f, 0.33f, 0.5f);
	}
    else
    {
		return newColor(1.0f, 0.0f, 0.0f, 0.5f);
	}
}

static inline unsigned char uclerp(unsigned char f1, unsigned char f2, unsigned char t)
{
	return f1*(255 - t) + f2*t;
}

void updateShape(void *ptr, void* unused)
{
cpShape *shape = (cpShape*)ptr;
}
static void DrawShape(cpShape *shape,  void* unused)
{

    cpBody *body = shape->body;
	Color color = ColorForBody(body);

	switch (shape->CP_PRIVATE(klass)->type)
    {
		case CP_CIRCLE_SHAPE:
        {
            cpCircleShape *circle = (cpCircleShape *)shape;
            cpVect center = circle->tc;
            cpFloat radius = circle->r;

            DrawCircle((int)center.x,(int)center.y,cpfmax(radius, 1.0),color);

            int linex0=(int)center.x;
            int liney0=(int)center.y;

            cpVect endline = cpvadd(center, cpvmult(body->rot, radius));

              int linex2=(int)endline.x;
              int liney2=(int)endline.y;

            DrawLine(linex0,liney0,linex2,liney2,color);

        }
             break;
		case CP_SEGMENT_SHAPE:
        {
            cpSegmentShape *seg = (cpSegmentShape *)shape;
          //  renderer->drawSegment(cpVert2Point(seg->ta), cpVert2Point(seg->tb), cpfmax(seg->r, 2.0), color);

            int linex0=(int)seg->ta.x;
            int liney0=(int)seg->ta.y;
            int linex2=(int)seg->tb.x;
            int liney2=(int)seg->tb.y;

         DrawLine(linex0,liney0,linex2,liney2,color);

        }
            break;
		case CP_POLY_SHAPE:
        {
            cpPolyShape *poly = (cpPolyShape *)shape;
            Color line  = Fade(color,0.5f);
           Vector2* pPoints = cpVertArray2ccpArrayN(poly->tVerts, poly->numVerts);
         //   renderer->drawPolygon(pPoints, poly->numVerts, color, 1.0, line);

         DrawTriangleFan(pPoints,poly->numVerts,line);
         DrawLineStrip(pPoints,poly->numVerts,color);
            SAFE_FREE(pPoints);
        }
            break;
		default:
			cpAssertHard(false, "Bad assertion in DrawShape()");
	}

}

static void DrawConstraint(cpConstraint *constraint,  void* unused)
{
    Color CONSTRAINT_COLOR = newColor(0,1,0,0.5f);

    cpBody *body_a = constraint->a;
	cpBody *body_b = constraint->b;

	const cpConstraintClass *klass = constraint->CP_PRIVATE(klass);
	if (klass == cpPinJointGetClass())
    {
		cpPinJoint *joint = (cpPinJoint *)constraint;

		cpVect a = cpBodyLocal2World(body_a, joint->anchr1);
		cpVect b = cpBodyLocal2World(body_b, joint->anchr2);



		int pointAX=(int)a.x;
		int pointAY=(int)a.y;
		int pointBX=(int)b.x;
		int pointBY=(int)b.y;


		DrawCircle(pointAX,pointAY,3.0f,CONSTRAINT_COLOR);
		DrawCircle(pointBX,pointBY,3.0f,CONSTRAINT_COLOR);


		DrawLine(pointAX,pointAY,pointBX,pointBY,CONSTRAINT_COLOR);

       // renderer->drawDot(cpVert2Point(a), 3.0, CONSTRAINT_COLOR);
       // renderer->drawDot(cpVert2Point(b), 3.0, CONSTRAINT_COLOR);
       // renderer->drawSegment(cpVert2Point(a), cpVert2Point(b), 1.0, CONSTRAINT_COLOR);
	}
    else if (klass == cpSlideJointGetClass())
    {
		cpSlideJoint *joint = (cpSlideJoint *)constraint;

		cpVect a = cpBodyLocal2World(body_a, joint->anchr1);
		cpVect b = cpBodyLocal2World(body_b, joint->anchr2);

     //   renderer->drawDot(cpVert2Point(a), 3.0, CONSTRAINT_COLOR);
      //  renderer->drawDot(cpVert2Point(b), 3.0, CONSTRAINT_COLOR);
      //  renderer->drawSegment(cpVert2Point(a), cpVert2Point(b), 1.0, CONSTRAINT_COLOR);


		int pointAX=(int)a.x;
		int pointAY=(int)a.y;
		int pointBX=(int)b.x;
		int pointBY=(int)b.y;


		DrawCircle(pointAX,pointAY,3.0f,CONSTRAINT_COLOR);
		DrawCircle(pointBX,pointBY,3.0f,CONSTRAINT_COLOR);

		DrawLine(pointAX,pointAY,pointBX,pointBY,CONSTRAINT_COLOR);
	}
    else if (klass == cpPivotJointGetClass())
    {
		cpPivotJoint *joint = (cpPivotJoint *)constraint;

		cpVect a = cpBodyLocal2World(body_a, joint->anchr1);
		cpVect b = cpBodyLocal2World(body_b, joint->anchr2);

		int pointAX=(int)a.x;
		int pointAY=(int)a.y;
		int pointBX=(int)b.x;
		int pointBY=(int)b.y;


		DrawCircle(pointAX,pointAY,3.0f,CONSTRAINT_COLOR);
		DrawCircle(pointBX,pointBY,3.0f,CONSTRAINT_COLOR);

		DrawLine(pointAX,pointAY,pointBX,pointBY,CONSTRAINT_COLOR);

      //  renderer->drawDot(cpVert2Point(a), 3.0, CONSTRAINT_COLOR);
      //  renderer->drawDot(cpVert2Point(b), 3.0, CONSTRAINT_COLOR);
	}
    else if (klass == cpGrooveJointGetClass())
    {
		cpGrooveJoint *joint = (cpGrooveJoint *)constraint;

		cpVect a = cpBodyLocal2World(body_a, joint->grv_a);
		cpVect b = cpBodyLocal2World(body_a, joint->grv_b);
		cpVect c = cpBodyLocal2World(body_b, joint->anchr2);


		int pointAX=(int)a.x;
		int pointAY=(int)a.y;
		int pointBX=(int)b.x;
		int pointBY=(int)b.y;
        int pointCX=(int)c.x;
		int pointCY=(int)c.y;



		DrawCircle(pointCX,pointCY,3.0f,CONSTRAINT_COLOR);

		DrawLine(pointAX,pointAY,pointBX,pointBY,CONSTRAINT_COLOR);

     //   renderer->drawDot(cpVert2Point(c), 3.0, CONSTRAINT_COLOR);
     //   renderer->drawSegment(cpVert2Point(a), cpVert2Point(b), 1.0, CONSTRAINT_COLOR);
	}
    else if (klass == cpDampedSpringGetClass())
    {
		// TODO
	}
    else
    {
        		printf("Cannot draw constraint\n");
	}
}



int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Create a space object
    cpSpace *space; // Holds our Space object

    space = cpSpaceNew();

    // Define a gravity vector

    space->gravity = cpv(0, 100);



cpBody *ballBody = cpBodyNew(100.0, INFINITY);
cpSpaceAddBody(space, ballBody);

cpShape *ballShape = cpCircleShapeNew(ballBody, 20.0, cpvzero);
ballShape->e = 0.9; // Elasticity
ballShape->u = 0.8; // Friction
ballShape->collision_type = 1; // Collisions are grouped by types
cpSpaceAddShape(space, ballShape);

ballBody->p = cpv(160, 20);



cpBody *floorBody = cpBodyNew(INFINITY, INFINITY);

floorBody->p = cpv(160, 350);

cpVect verts1[] = { cpv(0.0, 0.0), cpv(50.0, 0.0), cpv(45.0, -15.0), cpv(0.0, -15.0) };

cpVect verts2[] = { cpv(50.0, 0.0), cpv(116.0, -66.0), cpv(110.0, -81.0), cpv(45.0, -15.0) };

cpVect verts3[] = { cpv(116.0, -66.0), cpv(204.0, -66.0), cpv(210.0, -81.0), cpv(110.0, -81.0) };

cpVect verts4[] = { cpv(204.0, -66.0), cpv(270.0, 0.0), cpv(275.0, -15.0), cpv(210.0, -81.0) };

cpVect verts5[] = { cpv(270.0, 0.0), cpv(320.0, 0.0), cpv(320.0, -15.0), cpv(275.0, -15.0) };


cpShape *floorShape = cpPolyShapeNew(floorBody, 4, verts1, cpv(-320.0f / 2, 81.0f / 2));

floorShape->e = 0.5;
floorShape->u = 0.1;
floorShape->collision_type = 0;



cpSpaceAddStaticShape(space, floorShape);

floorShape = cpPolyShapeNew(floorBody, 4, verts2, cpv(-320.0f / 2, 81.0f / 2));

floorShape->e = 0.5;
floorShape->u = 0.1;
floorShape->collision_type = 0;


cpSpaceAddStaticShape(space, floorShape);

floorShape = cpPolyShapeNew(floorBody, 4, verts3, cpv(-320.0f / 2, 81.0f / 2));

floorShape->e = 0.5;
floorShape->u = 0.1;
floorShape->collision_type = 0;



cpSpaceAddStaticShape(space, floorShape);

floorShape = cpPolyShapeNew(floorBody, 4, verts4, cpv(-320.0f / 2, 81.0f / 2));

floorShape->e = 0.5;
floorShape->u = 0.1;
floorShape->collision_type = 0;



cpSpaceAddStaticShape(space, floorShape);

floorShape = cpPolyShapeNew(floorBody, 4, verts5, cpv(-320.0f / 2, 81.0f / 2));

floorShape->e = 0.5;
floorShape->u = 0.1;
floorShape->collision_type = 0;



cpSpaceAddStaticShape(space, floorShape);



// Add some elastic effects to the simulation

//space->elasticIterations = 10;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        cpSpaceStep(space, 1.0f/60.0f);


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(newColor(0,0,45,0));

     //   cpSpaceHashEach(space->, &updateShape, nullptr);

     cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)DrawShape, 0);
	 cpSpaceEachConstraint(space, (cpSpaceConstraintIteratorFunc)DrawConstraint, 0);


        DrawText("Djokersoft", 10, 30, 20, LIGHTGRAY);
        DrawFPS(10,10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    cpSpaceFree(space);
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
