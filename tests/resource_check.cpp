#include "font.h"
extern "C" {
#include "readrgb.h"
}
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

int main() {
    try {
        for(int size : {7,8,9,10,11,12,13,14,15,16,17,18,19,200}) {
            const std::string base=std::string(BIRDVIS_RESOURCE_DIR)+"/fonts/arial_"+std::to_string(size);
            Font *font=Font_load((base+".description").c_str(),(base+".sgi").c_str());
            if(!font || font->pt_size!=size || font->num_characters==0)
                throw std::runtime_error("Invalid font " + base);
            int w,h,components;
            auto *reference=reinterpret_cast<unsigned char *>(read_texture((base+".sgi").c_str(),&w,&h,&components));
            if(!reference || w!=font->texture->width || h!=font->texture->height)
                throw std::runtime_error("SGI size mismatch");
            for(int i=0;i<w*h;++i) if(font->texture->buffer[i]!=reference[4*i])
                throw std::runtime_error("SGI orientation/channel mismatch");
            free(reference); Font_free(font);
        }
        const auto bad=std::filesystem::temp_directory_path()/"birdvis-invalid-font.description";
        { std::ofstream file(bad); file << "broken font header\n"; }
        Font *font=Font_load(bad.c_str(),bad.c_str());
        std::filesystem::remove(bad);
        if(font) throw std::runtime_error("Corrupt font accepted");
        if(Image_read_sgi("/no-such-birdvis-image.sgi")) throw std::runtime_error("Missing SGI accepted");
        std::cout << "All font atlas pixels match legacy decoder; invalid resources rejected\n";
        return 0;
    } catch(const std::exception &e) { std::cerr << e.what() << '\n'; return 1; }
}
