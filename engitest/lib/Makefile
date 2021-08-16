
# ----------------------------------------------------------------------------------------------------
# ../Design/TestCase/DemoSKF
# ----------------------------------------------------------------------------------------------------
include ../../Config.make

SUB_DIR	:=

CFLAGS_CUR		=	$(CFLAGS) -DPRIVATE_CMD -DHB_CA_SKF
LDFLAGS_CUR		=	$(LDFLAGS)

ifeq ($(PRICMD), True)
	CFLAGS_CUR	:=	$(CFLAGS_CUR) -DPRICMDSTATE
endif

SRCS		=	$(wildcard *.c)
OBJS		=	$(patsubst %.c,%.o,$(wildcard *.c))


TARGET_FOLDER		=	../../BuildLinux/BuildDemoSKF/Bins/
DEMOSKF				=	$(TARGET_FOLDER)$(ARCH_BIT)Bit_APDUCMD/DemoSKF
DEMOSKF_STATIC		=	$(TARGET_FOLDER)$(ARCH_BIT)Bit_APDUCMD/DemoSKF_static
DEMOSKF_ADDR		=	$(TARGET_FOLDER)$(ARCH_BIT)Bit_APDUCMD/DemoSKF_addr
DEMOSKF_ADDR_STATIC	=	$(TARGET_FOLDER)$(ARCH_BIT)Bit_APDUCMD/DemoSKF_addr_static

SKF_PATH		=	../../BuildLinux/BuildSKF/Libs/$(ARCH_BIT)Bit_APDUCMD/

SKF_LIB			=	$(SKF_PATH)libSKF_final.a
SKF_LIB_ADDR	=	$(SKF_PATH)libSKF_final_addr.a
SKF				=	$(SKF_LIB) $(SKF_LIB_ADDR)

INCLUDES	=	-Iinclude/ \
				-I../../Src/SKF/include \
				-I../../Src/include


all	:	Init \
		$(SUB_DIR) \
		$(DEMOSKF) $(DEMOSKF_STATIC) \
		$(DEMOSKF_ADDR) $(DEMOSKF_ADDR_STATIC) \
		Final

Init:
	rm -rf $(TARGET_FOLDER)
	mkdir -p $(TARGET_FOLDER)
	mkdir -p $(TARGET_FOLDER)$(ARCH_BIT)Bit_APDUCMD

$(SUB_DIR)	:	ECHO
	make -C $@

ECHO	:
	@echo $(SUB_DIR)
	@echo Begin compile ......

# ------------------------------------------------------------------------------------------
# DemoSKF.
# ------------------------------------------------------------------------------------------
$(DEMOSKF)	:	$(SKF_LIB) $(OBJS)	
# Use libSKF_final.so
	$(CC) $(OBJS) $(LDFLAGS_CUR) -lSKF_final -L$(SKF_PATH) -o $(DEMOSKF)
# Use libSKF_final.a
#	$(CC) $(OBJS) $(CFLAGS_CUR) $(SKF_LIB) $(LDFLAGS_CUR) -o $(DEMOSKF)

$(DEMOSKF_STATIC)	:	$(SKF_LIB) $(OBJS)	
	$(CC) $(OBJS) $(CFLAGS_CUR) -static $(SKF_LIB) $(LDFLAGS_CUR) -o $(DEMOSKF_STATIC)
# ------------------------------------------------------------------------------------------
# End.
# ------------------------------------------------------------------------------------------


# ------------------------------------------------------------------------------------------
# DemoSKF_addr.
# ------------------------------------------------------------------------------------------
$(DEMOSKF_ADDR)	:	$(SKF_LIB_ADDR) $(OBJS)
# Use libSKF_final_addr.so
#	$(CC) $(OBJS) $(LDFLAGS_CUR) -lSKF_final_addr -L$(SKF_PATH) -o $(DEMOSKF_ADDR)
# Use libSKF_final_addr.a
	$(CC) $(OBJS) $(CFLAGS_CUR) $(SKF_LIB_ADDR) $(LDFLAGS_CUR) -o $(DEMOSKF_ADDR)

$(DEMOSKF_ADDR_STATIC)	:	$(SKF_LIB_ADDR) $(OBJS)
	$(CC) $(OBJS) $(CFLAGS_CUR) -static $(SKF_LIB_ADDR) $(LDFLAGS_CUR) -o $(DEMOSKF_ADDR_STATIC)

# ------------------------------------------------------------------------------------------
# End.
# ------------------------------------------------------------------------------------------

%.o	:	%.c
	$(CC) -c $(INCLUDES) $(CFLAGS_CUR) $< -o $@ 


$(SKF)	:
	make -C ../../Src/SKF
Final:


.PHONY : clean
clean:
	make clean -C ../../Src/SKF
	rm -f $(OBJS)
	rm -rf $(TARGET_FOLDER)

