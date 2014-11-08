#pragma once
// win
#include <Windows.h>
#include <Kinect.h>
// qt
#pragma warning(push)
#pragma warning(disable: 4127 4512 4800)
#include <QDebug>
#include <QImage>
#include <QMutex>
#include <QSharedPointer>
#include <QThread>
#include <QTime>
// vtk
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)
#include <QVTKWidget.h>
#include <QVTKApplication.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#pragma warning(pop)
