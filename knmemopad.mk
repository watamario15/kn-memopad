TARGET=AppMain.exe
CPP=clarm
CPPFLAGS=/nologo /W3 /O2 /EHsc /QRarch4T /QRinterwork-return \
	/D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=400 \
	/D "UNICODE" /D "_UNICODE" \
	/Ibrain-essential/knceutil /Ibrain-essential/kncedlg
LDFLAGS=/NOLOGO /SUBSYSTEM:WINDOWSCE
LIBS=commctrl.lib commdlg.lib brain-essential/knceutil/knceutil.lib \
    brain-essential/kncedlg/kncedlg.lib
OBJS=\
	knmemopad.obj
RESOURCE=knmemopad.res

all : $(TARGET)

$(TARGET) : $(OBJS) $(RESOURCE)
	link $(LDFLAGS) /OUT:$@ $(OBJS) $(RESOURCE) $(LIBS)

clean :
	-del $(TARGET) $(OBJS)
