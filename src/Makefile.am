ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -I./include $(DEVELOPER_CFLAGS)
AM_CXXFLAGS = -I./include $(DEVELOPER_CFLAGS)
EXTRA_DIST = include

bin_PROGRAMS = wxforms
wxforms_SOURCES = DTSApp.cpp DTSFrame.cpp DTSPanel.cpp evdata.cpp cxxapi.cpp dtsuserpw.c wxforms.c
wxforms_CFLAGS = $(AM_CFLAGS) $(WX_CFLAGS) $(DTSAPPLIB_CFLAGS)
wxforms_CXXFLAGS = $(AM_CXXFLAGS) $(WX_CXXFLAGS) $(DTSAPPLIB_CFLAGS)
wxforms_LDADD = $(WX_LIBS) $(DTSAPPLIB_LIBS)
