#include "Helper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
unsigned int ExistingTextures = 0;

bool Texture::Create()
{
    if (Created) return true;
    Binding.GLUID = CreateTexture(FilePath.c_str());
    if (Binding.GLUID == -1) 
    {
        std::cerr << "Failed to load texture: " << FilePath << std::endl;
        return false;
    }
    Created = true;
    Binding.TUID = ExistingTextures + GL_TEXTURE0;
    ExistingTextures++;
    std::cout << "Created texture: " << FilePath << ", GLUID: " << Binding.GLUID << ", TUID: " << Binding.TUID << std::endl;

    return true;
}

void Texture::BindTexture(unsigned int ShaderProgram)
{
    glActiveTexture(Binding.TUID);
    glBindTexture(GL_TEXTURE_2D, Binding.GLUID);
}

void BindTexture(TextureBinding Binding)
{
    glActiveTexture(Binding.TUID);
    glBindTexture(GL_TEXTURE_2D, Binding.GLUID);
}

void BlockBase::SetShaderTextures(unsigned int ShaderProgram)
{
    glUniform1i(100, TopFaceTexture.TUID - GL_TEXTURE0);
    glUniform1i(101, BottomFaceTexture.TUID - GL_TEXTURE0);
    glUniform1i(102, FrontFaceTexture.TUID - GL_TEXTURE0);
    glUniform1i(103, BackFaceTexture.TUID - GL_TEXTURE0);
    glUniform1i(104, LeftFaceTexture.TUID - GL_TEXTURE0);
    glUniform1i(105, RightFaceTexture.TUID - GL_TEXTURE0);
}

void BlockBase::FillTextures(TextureBinding Bindings[5])
{
    TopFaceTexture      = Bindings[0];
    BottomFaceTexture   = Bindings[1];
    FrontFaceTexture    = Bindings[2];                   
    BackFaceTexture     = Bindings[3];                
    LeftFaceTexture     = Bindings[4];                
    RightFaceTexture    = Bindings[5];
}

void BlockBase::FillTexturesStandardBlock(TextureBinding Bindings[3]) // create 3 textures: top, bottom, sides
{
    TopFaceTexture = Bindings[0];
    BottomFaceTexture = Bindings[1];
    FrontFaceTexture = Bindings[2];

    BackFaceTexture = FrontFaceTexture;
    LeftFaceTexture = FrontFaceTexture;
    RightFaceTexture = FrontFaceTexture;
}

void BlockBase::FillTexturesFromBlock(BlockBase ToCopy)
{
    TopFaceTexture =    ToCopy.TopFaceTexture;
    BottomFaceTexture = ToCopy.BottomFaceTexture;
    FrontFaceTexture =  ToCopy.FrontFaceTexture;
    BackFaceTexture =   ToCopy.BackFaceTexture;
    LeftFaceTexture =   ToCopy.LeftFaceTexture;
    RightFaceTexture =  ToCopy.RightFaceTexture;
}

unsigned int CreateTexture(const char* Path, int Type, int TextureRepeat, int MipmapSmoothing, int Smoothing, bool FlipVertically)
{
    stbi_set_flip_vertically_on_load(FlipVertically);
    unsigned int NewTexture;
    glGenTextures(1, &NewTexture);
    glBindTexture(GL_TEXTURE_2D, NewTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureRepeat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureRepeat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MipmapSmoothing);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Smoothing);

    int Width, Height, ChannelCount;
    unsigned char* Data = stbi_load(std::format("Textures/{}", Path).c_str(), &Width, &Height, &ChannelCount, 0);

    if (!Data) 
    {
        stbi_image_free(Data);
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, Type, Width, Height, 0, (ChannelCount == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(Data);

    return NewTexture;
}

void BindAllTextures(unsigned int ShaderProgram)
{
    for (const auto& [Name, Binding] : TextureMap) 
        BindTexture(Binding);
}