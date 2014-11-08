VTK_DIR = C:/soft/VTK
VTK_VER = 6.1

CONFIG += precompile_header
PRECOMPILED_HEADER = stdafx.h
CONFIG      += qt warn_on debug_and_release embed_manifest_exe thread
DEFINES += _CRT_SECURE_NO_WARNINGS
QMAKE_CXXFLAGS += /MP

TEMPLATE		=	app
HEADERS			+=	*.h
				
SOURCES			+= main.cpp kinect_reader.cpp depth_widget.cpp
				
INCLUDEPATH += 	"$$(KINECTSDK20_DIR)/inc" $${VTK_DIR}/include/vtk-$${VTK_VER}
LIBS += -L"$$(KINECTSDK20_DIR)/lib/x86" -lkinect20 -L$${VTK_DIR}/lib -lvtkGUISupportQt-$${VTK_VER} \
	-lvtkRenderingQt-$${VTK_VER} -lvtkCommonCore-$${VTK_VER} -lvtkRenderingCore-$${VTK_VER} -lvtkCommonExecutionModel-$${VTK_VER} -lvtkFiltersParallel-$${VTK_VER} \
	-lvtkFiltersGeneric-$${VTK_VER} -lvtkFiltersGeneral-$${VTK_VER} -lvtkFiltersGeometry-$${VTK_VER} -lvtkFiltersSources-$${VTK_VER} -lvtkInteractionWidgets-$${VTK_VER} \
	-lvtkInteractionStyle-$${VTK_VER} -lvtkRenderingOpenGL-$${VTK_VER} -lvtkRenderingFreeType-$${VTK_VER} -lvtkRenderingFreeTypeOpenGL-$${VTK_VER} \
	-lvtkCommonDataModel-$${VTK_VER}