/*=========================================================================

  Program:   Visualization Toolkit
  Module:    SpecularSpheres.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This examples demonstrates the effect of specular lighting.
//
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleFlight.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTerrain.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "vtkJPEGReader.h"
#include "vtkImageData.h"
#include <vtkAppendPolyData.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>
#include <vtkDepthSortPolyData.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include "Grid.cxx"

class WASDInteractorStyle : public vtkInteractorStyleFlight
{
    public:
	static WASDInteractorStyle* New();
	vtkTypeMacro(WASDInteractorStyle, vtkInteractorStyleFlight);
	
	vtkSmartPointer<vtkCamera> camera;
	vtkSmartPointer<vtkRenderWindow> renderWindow;

	virtual void OnKeyPress()
	{
		vtkSmartPointer<vtkTransform> left =
			vtkSmartPointer<vtkTransform>::New();
		vtkSmartPointer<vtkTransform> right =
			vtkSmartPointer<vtkTransform>::New();
		vtkSmartPointer<vtkTransform> forward =
			vtkSmartPointer<vtkTransform>::New();
		vtkSmartPointer<vtkTransform> backward =
			vtkSmartPointer<vtkTransform>::New();
		left->Identity();
		right->Identity();
		forward->Identity();
		backward->Identity();

		left->RotateZ(15.0);
		right->RotateZ(-15.0);
		
		double camPos[3], camFoc[3];


		vtkRenderWindowInteractor *rwi = this->Interactor;
		std::string key = rwi->GetKeySym();

		std::cout << "Pressed " << key << std::endl;


		if(key == "w")
		{
			camera->Dolly(10);
			camera->SetDistance(1);
/*
			camera->GetFocalPoint(camFoc);
			camera->GetPosition(camPos);
//			forward->Translate(camFoc);
//			forward->TransformPoint(camPos, camPos);
			camPos[0] = camPos[0]+(5*camFoc[0]/(abs(camFoc[0])+abs(camFoc[1])));
			camPos[1] = camPos[1]+(5*camFoc[1]/(abs(camFoc[0])+abs(camFoc[1])));
			camera->SetPosition(camPos);
*/
			renderWindow->Render();
		}
		if(key == "a")
		{
			camera->Azimuth(15);
/*
			std::cout << "Do the thing for " << key << std::endl;
			camera->GetFocalPoint(camFoc);
			left->TransformPoint(camFoc, camFoc);
			camera->SetFocalPoint(camFoc);
*/			renderWindow->Render();
			
		}
		if(key == "s")
		{
			camera->Dolly(0.1);
			camera->SetDistance(1);
/*			camera->GetFocalPoint(camFoc);
			camera->GetPosition(camPos);
			camPos[0] = camPos[0]-(5*camFoc[0]/(abs(camFoc[0])+abs(camFoc[1])));
		backward->Translate(camFoc);	
			camPos[1] = camPos[1]-(5*camFoc[1]/(abs(camFoc[0])+abs(camFoc[1])));
			camera->SetPosition(camPos);
*/
			renderWindow->Render();
		}
		if(key == "d")
		{
			camera->Azimuth(-15);
/*
			std::cout << "Do the thing for " << key << std::endl;
			camera->GetFocalPoint(camFoc);
			right->TransformPoint(camFoc, camFoc);
			camera->SetFocalPoint(camFoc);
*/
			renderWindow->Render();
		}
			camera->GetFocalPoint(camFoc);
			camera->GetPosition(camPos);
			printf("Cam %f %f %f looking %f %f %f\n", camPos[0], camPos[1], camPos[2], camFoc[0], camFoc[1], camFoc[2]);
	
		//vtkInteractorStyleFlightCamera::OnKeyPress();	
	}
};

vtkStandardNewMacro(WASDInteractorStyle);


class vtk441Mapper : public vtkOpenGLPolyDataMapper
{
    protected:
	GLuint displayList;
	bool initialized;
    public:
	vtk441Mapper()
	{
	    initialized = false;
	}

	void
	RemoveVTKOpenGLStateSideEffects()
	{
		float Info[4] = {0, 0, 0, 1};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Info);
		float ambient[4] = {1, 1, 1, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		float diffuse[4] = {1, 1, 1, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		float specular[4] = {1, 1, 1, 1.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	}

	void SetupLight(void)
	{
	       glEnable(GL_LIGHTING);
	       glEnable(GL_LIGHT0);
	       GLfloat diffuse0[4] = { 0.8, 0.8, 0.8, 1 };
	       GLfloat ambient0[4] = { 0.2, 0.2, 0.2, 1 };
	       GLfloat specular0[4] = { 0.0, 0.0, 0.0, 1 };
	       GLfloat pos0[4] = { 1, 2, 13, 0 };
	       GLfloat pos1[4] = { 10*M, 2, 13, 0 };
	       GLfloat pos2[4] = { 10*M/2, 10*M/2, 13, 0 };
	       GLfloat pos3[4] = { 1, 10*M, 13, 0 };
	       GLfloat pos4[4] = { 10*M, 10*M, 13, 0 };
	       glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	       glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	       glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	       glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	       glEnable(GL_LIGHT1);
	       glLightfv(GL_LIGHT1, GL_POSITION, pos1);
	       glEnable(GL_LIGHT2);
	       glLightfv(GL_LIGHT2, GL_POSITION, pos2);
	       glEnable(GL_LIGHT3);
	       glLightfv(GL_LIGHT3, GL_POSITION, pos3);
	       glEnable(GL_LIGHT4);
	       glLightfv(GL_LIGHT4, GL_POSITION, pos4);
//		GLfloat *pos[9] = {pos0, pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8};
//		GL_LIGHTS *light[9] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7, GL_LIGHT8};
/*		for (int i = 0; i < 9; i++)
		{
			glEnable(light[i]);
			glLightfv(light[i], GL_POSITION, pos[i]);
		}
	       glEnable(GL_LIGHT1);
	       glEnable(GL_LIGHT2);
	       glEnable(GL_LIGHT3);
*/	       glDisable(GL_LIGHT5);
	       glDisable(GL_LIGHT6);
	       glDisable(GL_LIGHT7);

	}
};

class vtk441MapperPart1 : public vtk441Mapper
{
 public:
   static vtk441MapperPart1 *New();
   Grid Game;


   virtual void RenderPiece(vtkRenderer *ren, vtkActor *act)
   {
      vtkJPEGReader *rdr = vtkJPEGReader::New();
	rdr->SetFileName("Arrow.jpg");
	rdr->Update();
	vtkImageData *img = rdr->GetOutput();
	int dims[3];
	img->GetDimensions(dims);
	unsigned char *buffer = (unsigned char *) img->GetScalarPointer(0,0,0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dims[0], dims[1], 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	glEnable(GL_COLOR_MATERIAL);
		
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnable(GL_TEXTURE_2D);

      RemoveVTKOpenGLStateSideEffects();
      SetupLight();

	glBegin(GL_QUADS);
	      glVertex3f(0, 0, 0);
	      glVertex3f(M*10, 0, 0);
	      glVertex3f(M*10, M*10, 0);
	      glVertex3f(0, M*10, 0);
	glEnd();

	for (int i = 0; i < M*M; i++)
	{
		if(Game.Tiles[i].Walls[1])
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0,1);
			glVertex3f(((i%M)*10)+0.05, ((i/M)*10)+0.05, 0);
			glTexCoord2f(0,0);
			glVertex3f(((i%M)*10)+0.05, (((i/M)+1)*10)-0.05, 0);
			glTexCoord2f(1,0);
			glVertex3f(((i%M)*10)+0.05, (((i/M)+1)*10)-0.05, 10);
			glTexCoord2f(1,1);
			glVertex3f(((i%M)*10)+0.05, ((i/M)*10)+0.05, 10);
			glEnd();
		}
		if(Game.Tiles[i].Walls[2])
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(((i%M)*10)+0.05, (((i/M)+1)*10)-0.05, 0);
			glTexCoord2f(1,0);
			glVertex3f((((i%M)+1)*10)-0.05, (((i/M)+1)*10)-0.05, 0);
			glTexCoord2f(1,1);
			glVertex3f((((i%M)+1)*10)+0.05, (((i/M)+1)*10)-0.05, 10);
			glTexCoord2f(0,1);
			glVertex3f(((i%M)*10)-0.05, (((i/M)+1)*10)-0.05, 10);
			glEnd();
		}
		if(Game.Tiles[i].Walls[3])
		{
			glBegin(GL_QUADS);
			glTexCoord2f(1,0);
			glVertex3f((((i%M)+1)*10)-0.05, ((i/M)+1)*10, 0);
			glTexCoord2f(1,1);
			glVertex3f((((i%M)+1)*10)-0.05, ((i/M))*10, 0);
			glTexCoord2f(0,1);
			glVertex3f((((i%M)+1)*10)-0.05, ((i/M))*10, 10);
			glTexCoord2f(0,0);
			glVertex3f((((i%M)+1)*10)-0.05, ((i/M)+1)*10, 10);
			glEnd();
		}
		if(Game.Tiles[i].Walls[4])
		{
			glBegin(GL_QUADS);
			glTexCoord2f(1,1);
			glVertex3f((((i%M)+1)*10)-0.05, (((i/M))*10)+0.05, 0);
			glTexCoord2f(1,0);
			glVertex3f(((i%M)*10)+0.05, ((i/M)*10)+0.05, 0);
			glTexCoord2f(0,0);
			glVertex3f(((i%M)*10)+0.05, ((i/M)*10)+0.05, 10);
			glTexCoord2f(0,1);
			glVertex3f((((i%M)+1)*10)-0.05, (((i/M))*10)+0.05, 10);
			glEnd();
		}
/*
	      glVertex3f(0, 0, 0);
	      glVertex3f(100, 0, 0);
	      glVertex3f(100, 100, 0);
	      glVertex3f(0, 100, 0);
*/
	}
//      glVertex3f(-0.10, -10, 10);
//      glVertex3f(10, -10, 10);
//      glVertex3f(10, 10, 10);
//      glVertex3f(-10, 10, 10);
//      glVertex3f(10, 10, 30);
//      glVertex3f(-10, 10, 30);
   }
};

vtkStandardNewMacro(vtk441MapperPart1);

int main()
{
//GROD MAIN
	srand(time(NULL));
	std::cout<<"Main Gen Game\n";
	Grid Game1 = Grid(); 	// Function for activating outer walls and top row of tiles
	std::cout << "Print Maze\n";
	Game1.PrintMaze();      	// Fn for printing Grid ‾_|‾|_| 	
//END GRID MAIN
	vtkSmartPointer<vtkSphereSource> sphere =
		vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetThetaResolution(100);
	sphere->SetPhiResolution(100);
	
	vtkSmartPointer<vtk441MapperPart1> win1Mapper =
	    vtkSmartPointer<vtk441MapperPart1>::New();
	win1Mapper->SetInputConnection(sphere->GetOutputPort());
	
	win1Mapper->Game = Game1;

	vtkSmartPointer<vtkActor> win1Actor =
	    vtkSmartPointer<vtkActor>::New();
	win1Actor->SetMapper(win1Mapper);

	vtkSmartPointer<vtkRenderer> ren1 =
	    vtkSmartPointer<vtkRenderer>::New();

	//Sets up camera
	vtkSmartPointer<vtkRenderWindow> renWin =
	    vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer(ren1);
	ren1->SetViewport(0, 0, 1, 1);

	vtkSmartPointer<vtkRenderWindowInteractor> iren =
	    vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	bool doWindow1 = true;
	if (doWindow1)
		ren1->AddActor(win1Actor);
	ren1->SetBackground(0.0, 0.0, 0.0);
	
	vtkSmartPointer<vtkLight> Hlight = vtkSmartPointer<vtkLight>::New();
	Hlight->SetLightTypeToHeadlight();
	
	renWin->SetSize(1000, 1000);

	ren1->GetActiveCamera()->SetFocalPoint(0,0, 0);
	ren1->GetActiveCamera()->SetPosition(100,195,200);
	ren1->GetActiveCamera()->SetViewUp(0,0,1);
	ren1->GetActiveCamera()->SetClippingRange(.5,1000);
	ren1->GetActiveCamera()->SetViewAngle(45);
	ren1->GetActiveCamera()->SetDistance(1);
	ren1->AddLight(Hlight);
	ren1->SetLightFollowCamera(1);
	
	vtkSmartPointer<WASDInteractorStyle> style =
		vtkSmartPointer<WASDInteractorStyle>::New();
	
	style->camera = ren1->GetActiveCamera();
	style->renderWindow = renWin;
//	vtkSmartPointer<vtkInteractorStyleFlight> style =
//		vtkSmartPointer<vtkInteractorStyleFlight>::New();
//		style.OnKeyDown()
	iren->SetInteractorStyle(style);

	((vtkInteractorStyleFlight *)iren->GetInteractorStyle())->SetAutoAdjustCameraClippingRange(0);
	iren->Initialize();
	iren->Start();

	return EXIT_SUCCESS;
}

