#include "Helper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
unsigned int ExistingTextures = 0;

bool Texture::Create()
{
    if (Created) return true;
    GLUID = CreateTexture(FilePath.c_str());
    if (GLUID == -1)
    {
        std::cout << "GLUID == -1 for texture: " << this->Name << std::endl;
        return false;
    }
    Created = true;
    TUID = ExistingTextures + GL_TEXTURE0;
    ExistingTextures++;

    return true;
}

bool BlockBase::CreateTextures(unsigned int ShaderProgram)
{
    if (!TopFaceTexture.Create() || !BottomFaceTexture.Create() || !FrontFaceTexture.Create() || !BackFaceTexture.Create() || !LeftFaceTexture.Create() || !RightFaceTexture.Create())
        return false;

    return true;
}

void BlockBase::BindTextures(unsigned int ShaderProgram)
{
    glUseProgram(ShaderProgram);

    glActiveTexture(TopFaceTexture.TUID);
    glBindTexture(GL_TEXTURE_2D, TopFaceTexture.GLUID);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Top"), TopFaceTexture.TUID - GL_TEXTURE0);

    glActiveTexture(BottomFaceTexture.TUID);
    glBindTexture(GL_TEXTURE_2D, BottomFaceTexture.GLUID);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Bottom"), BottomFaceTexture.TUID - GL_TEXTURE0);

    glActiveTexture(FrontFaceTexture.TUID);
    glBindTexture(GL_TEXTURE_2D, FrontFaceTexture.GLUID);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Front"), FrontFaceTexture.TUID - GL_TEXTURE0);

    glActiveTexture(BackFaceTexture.TUID);
    glBindTexture(GL_TEXTURE_2D, BackFaceTexture.GLUID);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Back"), BackFaceTexture.TUID - GL_TEXTURE0);

    glActiveTexture(LeftFaceTexture.TUID);
    glBindTexture(GL_TEXTURE_2D, LeftFaceTexture.GLUID);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Left"), LeftFaceTexture.TUID - GL_TEXTURE0);

    glActiveTexture(RightFaceTexture.TUID);
    glBindTexture(GL_TEXTURE_2D, RightFaceTexture.GLUID);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Right"), RightFaceTexture.TUID - GL_TEXTURE0);
}

void BlockBase::FillTextures(std::string Names[4], std::string Paths[4])
{
    TopFaceTexture = Texture();
    TopFaceTexture.Name = Names[0];
    TopFaceTexture.FilePath = Paths[0];

    BottomFaceTexture = Texture();
    BottomFaceTexture.Name = Names[1];
    BottomFaceTexture.FilePath = Paths[1];

    FrontFaceTexture = Texture();
    FrontFaceTexture.Name = Names[2];
    FrontFaceTexture.FilePath = Paths[2];

    BackFaceTexture = Texture();
    BackFaceTexture.Name = Names[3];
    BackFaceTexture.FilePath = Paths[3];

    LeftFaceTexture = Texture();
    LeftFaceTexture.Name = Names[4];
    LeftFaceTexture.FilePath = Paths[4];

    RightFaceTexture = Texture();
    RightFaceTexture.Name = Names[5];
    RightFaceTexture.FilePath = Paths[5];
}

void BlockBase::FillTexturesStandardBlock(std::string Names[2], std::string Paths[2]) // create 3 textures: top, bottom, sides
{
    TopFaceTexture = Texture();
    TopFaceTexture.Name = Names[0];
    TopFaceTexture.FilePath = Paths[0];

    BottomFaceTexture = Texture();
    BottomFaceTexture.Name = Names[1];
    BottomFaceTexture.FilePath = Paths[1];

    FrontFaceTexture = Texture();
    FrontFaceTexture.Name = Names[2];
    FrontFaceTexture.FilePath = Paths[2];

    BackFaceTexture = FrontFaceTexture;
    LeftFaceTexture = FrontFaceTexture;
    RightFaceTexture = FrontFaceTexture;
}

void BlockBase::FillTexturesStandardBlockByPaths(std::string Paths[2]) // create 3 textures: top, bottom, sides
{
    TopFaceTexture = Texture();
    TopFaceTexture.Name = Paths[0];
    TopFaceTexture.FilePath = Paths[0];

    BottomFaceTexture = Texture();
    BottomFaceTexture.Name = Paths[1];
    BottomFaceTexture.FilePath = Paths[1];

    FrontFaceTexture = Texture();
    FrontFaceTexture.Name = Paths[2];
    FrontFaceTexture.FilePath = Paths[2];

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

unsigned int CreateTexture(const char* Path, int Type , int TextureRepeat, int MipmapSmoothing, int Smoothing, bool FlipVertically)
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
        return -1;

    glTexImage2D(GL_TEXTURE_2D, 0, Type, Width, Height, 0, Type, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(Data);

    return NewTexture;
}