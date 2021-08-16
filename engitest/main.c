#include <stdio.h>
#include <stdarg.h>


int main()
{
    UINT32 lLoopNum = 1, i = 0;
    BYTE c;

    SKFE_IssueCard();

    get_hw_sw_info();

    test_all_skf();


#ifndef LINUX
    system("pause");
#endif
    return 0;
}


void test_all_skf()
{
    UINT32 lLoopNum = 1, i = 0;
    char c = 1;

#ifndef LINUX
    remove("Demo_log.txt");
#endif
    for (i = 0; i < lLoopNum; ++i)
    {
        //Android 64位必须这么打印，否则有问题
        PrintMsg("/**********************");
        PrintMsg("%d",i+1);
        PrintMsg(" test******************************/\n");
        PrintMsg("===============================\n");
        dev_manage_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_control_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_appliction_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_file_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_container_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_sym_hash_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_ecc_funs_test();
        PrintMsg("===============================\n\n");
        PrintMsg("===============================\n");
        dev_rsa_funs_test();
        PrintMsg("===============================\n\n");
        /*PrintMsg("===============================\n");
        alg_performances_test();
        PrintMsg("===============================\n\n");*/

        SKF_DeleteContainer(hApplication, "Test_Container");
        SKF_CloseApplication(hApplication);
        SKF_DeleteApplication(hDev, "Test_Application");

        PrintMsg("===============================\n");
        dev_skf_expand_interafce_test();
        PrintMsg("===============================\n\n");

        SKF_DisConnectDev(hDev);
        PrintMsg("===============================\n");
        EccGenerateKeyInterfaceTest();
        PrintMsg("===============================\n\n");
    }
    PrintMsg("Test complete\n");
#ifndef LINUX
    system("pause");
#endif
}