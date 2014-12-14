VTK_DIR = E:/soft/VTK
VTK_VER = 6.1

OPENCV_DIR = E:/soft/opencv-3.0.0b/build
OPENCV_VER = 300

ZBAR_DIR = E:/soft/ZBar

CONFIG += precompile_header
PRECOMPILED_HEADER = stdafx.h
CONFIG      += qt warn_on debug_and_release embed_manifest_exe thread
DEFINES += _CRT_SECURE_NO_WARNINGS

TEMPLATE		=	app
HEADERS			+=	*.h
				
SOURCES			+= main.cpp depth_widget.cpp
				
INCLUDEPATH += 	$${VTK_DIR}/include/vtk-$${VTK_VER} \
                $${OPENCV_DIR}/include \
                $${ZBAR_DIR}/include

LIBS += -L$${VTK_DIR}/lib -lvtkGUISupportQt-$${VTK_VER} \
	-lvtkRenderingQt-$${VTK_VER} -lvtkCommonCore-$${VTK_VER} -lvtkRenderingCore-$${VTK_VER} -lvtkCommonExecutionModel-$${VTK_VER} -lvtkFiltersParallel-$${VTK_VER} \
	-lvtkFiltersGeneric-$${VTK_VER} -lvtkFiltersGeneral-$${VTK_VER} -lvtkFiltersGeometry-$${VTK_VER} -lvtkFiltersSources-$${VTK_VER} -lvtkInteractionWidgets-$${VTK_VER} \
	-lvtkInteractionStyle-$${VTK_VER} -lvtkRenderingOpenGL-$${VTK_VER} -lvtkRenderingFreeType-$${VTK_VER} -lvtkRenderingFreeTypeOpenGL-$${VTK_VER} \
	-lvtkCommonDataModel-$${VTK_VER} \
    -l$${ZBAR_DIR}/lib/libzbar-0 \
    -l$${OPENCV_DIR}/x86/vc12/lib/opencv_world300d -l$${OPENCV_DIR}/x86/vc12/lib/opencv_ts300d