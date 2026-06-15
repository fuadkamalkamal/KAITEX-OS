#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Multiboot Info Structure */
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    
    /* Video Info (VBE) */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    /* Framebuffer Info (Multiboot 1, valid if flags bit 12 is set) */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    union {
        struct {
            uint32_t framebuffer_palette_addr;
            uint16_t framebuffer_palette_num_colors;
        };
        struct {
            uint8_t framebuffer_red_field_position;
            uint8_t framebuffer_red_mask_size;
            uint8_t framebuffer_green_field_position;
            uint8_t framebuffer_green_mask_size;
            uint8_t framebuffer_blue_field_position;
            uint8_t framebuffer_blue_mask_size;
        };
    };
} __attribute__((packed)) multiboot_info_t;

static uint32_t* framebuffer = NULL;
static uint32_t fb_width = 0;
static uint32_t fb_height = 0;
static uint32_t fb_pitch = 0;
static uint8_t fb_bpp = 0;

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_width || y >= fb_height || framebuffer == NULL) return;
    
    // Pitch adalah ukuran satu baris dalam byte, kita pakai uint32_t pointer (4 byte)
    uint32_t offset = (y * fb_pitch / 4) + x;
    framebuffer[offset] = color;
}

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

/* KERNEL ENTRY POINT */
void kernel_main(uint32_t magic, multiboot_info_t* mbi) {
    // Memastikan diboot oleh Multiboot compliant bootloader (GRUB)
    if (magic != 0x2BADB002) {
        return; 
    }
    
    // Cek apakah bit 12 diset (Framebuffer info tersedia dari GRUB)
    if (mbi->flags & (1 << 12)) {
        framebuffer = (uint32_t*) (uint32_t) mbi->framebuffer_addr;
        fb_width = mbi->framebuffer_width;
        fb_height = mbi->framebuffer_height;
        fb_pitch = mbi->framebuffer_pitch;
        fb_bpp = mbi->framebuffer_bpp;
    } else {
        return; // Tidak ada mode grafis, gagal menggambar GUI
    }

    // 1. Gambar Background Penuh (Warna Biru Muda / Light Blue)
    // Warna hex: 0x00AEEF
    draw_rect(0, 0, fb_width, fb_height, 0x00AEEF);

    // 2. Gambar Jendela (Window) Putih di tengah
    uint32_t win_width = 600;
    uint32_t win_height = 400;
    uint32_t win_x = (fb_width - win_width) / 2;
    uint32_t win_y = (fb_height - win_height) / 2;
    
    // Border Jendela (Abu-abu)
    draw_rect(win_x - 2, win_y - 2, win_width + 4, win_height + 4, 0x555555);
    // Isi Jendela (Putih)
    draw_rect(win_x, win_y, win_width, win_height, 0xFFFFFF);
    
    // 3. Title bar Jendela (Biru Tua)
    uint32_t title_height = 30;
    draw_rect(win_x, win_y, win_width, title_height, 0x0055AA);
    
    // 4. Tombol Close (Silang) Merah
    uint32_t close_size = 20;
    uint32_t close_x = win_x + win_width - close_size - 5;
    uint32_t close_y = win_y + 5;
    draw_rect(close_x, close_y, close_size, close_size, 0xFF0000);

    // Hentikan prosesor (idle loop)
    while(1) {
        __asm__ __volatile__("hlt");
    }
}
