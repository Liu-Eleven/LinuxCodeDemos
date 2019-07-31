####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qvfb

## INCLUDES were found outside kdevelop specific part

qvfb_SOURCES = skin.cpp qvfbview.cpp qvfbratedlg.cpp qvfb.cpp qanimationwriter.cpp main.cpp config.ui 
SOURCES = skin.cpp qvfbview.cpp qvfbratedlg.cpp qvfb.cpp qanimationwriter.cpp main.cpp 
INTERFACES = config.ui 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
qvfb_LDADD   =  $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = config.ui gammaview.h main.cpp pda_down.png pda.skin pda_up.png qanimationwriter.cpp qanimationwriter.h qvfb.cpp qvfb.h qvfbratedlg.cpp qvfbratedlg.h qvfbview.cpp qvfbview.h README skin.cpp skin.h 

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

qvfb_METASOURCES = AUTO

# the library search path. 
qvfb_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qvfb_METASOURCES)

