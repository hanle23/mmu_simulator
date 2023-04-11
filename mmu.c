#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PAGE_TABLE_COUNT 256
#define PAGE_TABLE_PHYSICAL_SIZE 256
#define TLB_COUNT 16
#define TLB_FRAME_PHYSICAL_SIZE 256
#define FRAME_COUNT 256

struct TLB
{
    int page_number;
    int frame_number;
};

struct PageTable
{
    int value;
    int valid;
    int waiting_time;
};

int search_TLB(int page_number);
int search_PageTable(int page_number);
void update_PageTable(int page_number, int new_frame_number);
void update_TLB(int page_number);
int longest_unused_in_pagetable();
void increase_waiting_time();
void invalue_old_data();
void search_physical_address();

struct TLB tlb[TLB_COUNT];
struct PageTable page_table[PAGE_TABLE_COUNT];
int total_count = 0;
int hit_count = 0;
int page_fault_count = 0;
int free_frame_index = 0;
int tlb_current_index = 0;
char *output;
int physical_address_space = 0;
char physical_memory[256][256];

int main(int argc, char *argv[])
{
    int PHYSICAL_SIZE = atoi(argv[1]);
    if (PHYSICAL_SIZE == 256)
    {
        output = "output256.csv";
    }
    else
    {
        output = "output128.csv";
    }

    for (int i = 0; i < PAGE_TABLE_COUNT; i++)
    {
        page_table[i].valid = 0;
        page_table[i].value = 0;
        page_table[i].waiting_time = -1;
    }

    int logical_addresses;
    int offset;
    int page_number;
    int virtual_page_number;
    int frame_number;

    FILE *output_file = fopen(output, "w");
    FILE *backing_store_file = fopen(argv[2], "rb");
    FILE *addr_file = fopen(argv[3], "r");

    while (fscanf(addr_file, "%d", &logical_addresses) != EOF)
    {
        total_count++;

        offset = logical_addresses & 0xff;
        page_number = (logical_addresses >> 8) & 0xff;
        virtual_page_number = logical_addresses & 0xff00;

        char physical_memory_offset[FRAME_COUNT];

        frame_number = search_TLB(page_number);
        if (frame_number != -1)
        {
            hit_count++;
        }
        else
        {
            frame_number = search_PageTable(page_number);
            if (frame_number == -1)
            {
                page_fault_count++;

                fseek(backing_store_file, virtual_page_number, SEEK_SET);
                fread(physical_memory_offset, 1, 256, backing_store_file);
                search_physical_address(physical_memory_offset, page_number, PHYSICAL_SIZE);

                frame_number = search_PageTable(page_number);
            }
            update_TLB(page_number);
        }

        increase_waiting_time();
        page_table[page_number].waiting_time = 0;

        int physical_addresses = (page_table[page_number].value << 8) + offset;
        int signed_byte_value = physical_memory[page_table[page_number].value][offset];
        fprintf(output_file, "%d,%d,%d\n", logical_addresses, physical_addresses, signed_byte_value);
    }

    fprintf(output_file, "Page Faults Rate, %.2f%%,\n", (float)page_fault_count / (float)total_count * 100.00);
    fprintf(output_file, "TLB Hits Rate, %.2f%%,", (float)hit_count / (float)total_count * 100.00);

    fclose(addr_file);
    fclose(output_file);
    fclose(backing_store_file);

    return 0;
}

int search_TLB(int page_number)
{
    int index = -1;

    for (int i = 0; i < TLB_COUNT; i++)
    {
        if (tlb[i].page_number == page_number)
        {
            index = i;
        }
    }
    if (index != -1)
    {
        return tlb[index].frame_number;
    }
    else
    {
        return index;
    }
}

int search_PageTable(int page_number)
{
    int index = -1;

    for (int i = 0; i < PAGE_TABLE_COUNT; i++)
    {
        if (i == page_number && page_table[i].valid == 1)
        {
            index = i;
        }
    }
    if (index != -1)
    {
        return page_table[index].value;
    }
    else
    {
        return index;
    }
}

void update_PageTable(int page_number, int new_frame_number)
{
    page_table[page_number].value = new_frame_number;
    page_table[page_number].valid = 1;
    page_table[page_number].waiting_time = 0;
    free_frame_index++;
}

void update_TLB(int page_number)
{
    tlb[tlb_current_index].page_number = page_number;
    tlb[tlb_current_index].frame_number = search_PageTable(page_number);
    tlb_current_index = (tlb_current_index + 1) % TLB_COUNT;
}

int longest_unused_in_pagetable()
{
    int longest_unused_page_number = 0;
    for (int i = 0; i < PAGE_TABLE_COUNT; i++)
    {
        if (page_table[i].waiting_time > page_table[longest_unused_page_number].waiting_time)
        {
            longest_unused_page_number = i;
        }
    }
    return longest_unused_page_number;
}

void increase_waiting_time()
{
    for (int i = 0; i < PAGE_TABLE_COUNT; i++)
    {
        if (page_table[i].waiting_time >= 0)
        {
            page_table[i].waiting_time++;
        }
    }
}

void invalue_old_data(int page_number)
{
    page_table[page_number].valid = 0;
    page_table[page_number].value = 0;
    page_table[page_number].waiting_time = -1;
}

void search_physical_address(char memory_offset[], int page_number, int MEMORY_SIZE)
{
    if (MEMORY_SIZE == 256)
    {
        for (int i = 0; i < 256; i++)
        {
            physical_memory[free_frame_index][i] = memory_offset[i];
        }

        update_PageTable(page_number, free_frame_index);
    }
    else if (MEMORY_SIZE == 128)
    {
        int new_frame = 0;
        int oldest_page_number = 0;
        if (free_frame_index < 128)
        {
            new_frame = free_frame_index;
        }
        else
        {
            oldest_page_number = longest_unused_in_pagetable();
            new_frame = page_table[oldest_page_number].value;
            invalue_old_data(oldest_page_number);
        }

        for (int i = 0; i < 256; i++)
        {
            physical_memory[new_frame][i] = memory_offset[i];
        }
        update_PageTable(page_number, new_frame);
    }
}