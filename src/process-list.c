#include "vmi.h"

int introspect_process_list (char *name) {
    vmi_instance_t vmi;
    addr_t list_head = 0, next_list_entry = 0, current_process = 0;
    vmi_pid_t pid = 0;
    char *procname = NULL;

    if (vmi_init(&vmi, VMI_XEN | VMI_INIT_COMPLETE, name) == VMI_FAILURE) {
        printf("Failed to init LibVMI library.\n");
        return 1;
    }

    vmi_pause_vm(vmi);

    /**
     * get offsets of the kernel data structures
     * get the head of the task_struct 
     */

    switch(vmi_get_ostype(vmi)) {
        case VMI_OS_LINUX:
            tasks_offset = vmi_get_offset(vmi, "linux_tasks");
            name_offset = vmi_get_offset(vmi, "linux_name");
            pid_offset = vmi_get_offset(vmi, "linux_pid");

            list_head = vmi_translate_ksym2v(vmi, "init_task") + tasks_offset;

            break;
        case VMI_OS_WINDOWS:
            tasks_offset = vmi_get_offset(vmi, "win_tasks");
            name_offset = vmi_get_offset(vmi, "win_pname");
            pid_offset = vmi_get_offset(vmi, "win_pid");

            list_head = vmi_translate_ksym2v(vmi, "PsActiveProcessHead");

            break;
        default:
            goto exit;
    }


    if (tasks_offset == 0 || pid_offset == 0 || name_offset == 0) {
        printf("Failed to find offsets\n");
        goto exit;
    }

    next_list_entry = list_head;

    /** 
     * traverse the task lists and print out each process 
     */
    do {
        current_process = next_list_entry - tasks_offset;
        vmi_read_32_va(vmi, current_process + pid_offset, 0, (uint32_t*)&pid);
        procname = vmi_read_str_va(vmi, current_process + name_offset, 0);
        if (!procname) {
            printf("Failed to find procname\n");
            goto exit;
        }

        printf("[%5d] %s\n", pid, procname);

        free(procname);
        procname = NULL;

        if (vmi_read_addr_va(vmi, next_list_entry, 0, &next_list_entry) == VMI_FAILURE) {
            printf("Failed to read next pointer in loop at %"PRIx64"\n", next_list_entry);
            goto exit;
        }

    } while(next_list_entry != list_head);

exit:
    vmi_resume_vm(vmi);
    vmi_destroy(vmi);

    return 0;
}
