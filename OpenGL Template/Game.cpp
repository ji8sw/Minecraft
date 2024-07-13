#include "Helper.h"

float CubeVertices[] =
{
	// Front face
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	// Back face
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	// Left face
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	// Right face
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 // Bottom face
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	 // Top face
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


GLFWwindow* Window = NULL;
unsigned int VBO, VAO, EBO;
unsigned int VertexShader;
unsigned int FragmentShader;
unsigned int ShaderProgram;
bool WireframeMode = false;
int ExistingTextures = 0;
glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 15.0f);
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
double LastTime, FPS, MS;
int Count = 0;
float CursorX = 1920 / 2, CursorY = 1080 / 2;
bool FirstMouse = true, InMenu = false;
float CameraYaw = -90.0f, CameraPitch = 0.0f;
typedef std::vector<std::vector<std::vector<Block>>> chunkData;
typedef std::map<std::pair<int, int>, chunkData> ExistingChunksMap;
ExistingChunksMap ExistingChunks;

void WindowSizeChanged(GLFWwindow* Window, int NewWidth, int NewHeight)
{
	glViewport(0, 0, NewWidth, NewHeight);
}

void MouseMoved(GLFWwindow* Window, double NewXPosition, double NewYPosition)
{
	if (FirstMouse)
	{
		CursorX = NewXPosition;
		CursorY = NewYPosition;
		FirstMouse = false;
	}

	float XOffset = NewXPosition - CursorX;
	float YOffset = CursorY - NewYPosition; // reversed since y-coordinates range from bottom to top
	CursorX = NewXPosition;
	CursorY = NewYPosition;

	if (!InMenu)
	{
		const float Sensitivity = 0.05f;
		XOffset *= Sensitivity;
		YOffset *= Sensitivity;

		CameraYaw += XOffset;
		CameraPitch += YOffset;

		if (CameraPitch > 89.0f)
			CameraPitch = 89.0f;
		if (CameraPitch < -89.0f)
			CameraPitch = -89.0f;

		glm::vec3 Front;
		Front.x = cos(glm::radians(CameraYaw)) * cos(glm::radians(CameraPitch));
		Front.y = sin(glm::radians(CameraPitch));
		Front.z = sin(glm::radians(CameraYaw)) * cos(glm::radians(CameraPitch));
		CameraFront = glm::normalize(Front);
	}
}

bool InitializeGL(int Major = 3, int Minor = 3)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return true;
}

bool MakeWindow(int Width = 1920, int Height = 1080, const char* Title = "Minecraft", bool MakeContextCurrent = true, bool DefViewport = true)
{
	Window = glfwCreateWindow(Width, Height, Title, NULL, NULL);
	if (Window == NULL)
		return false;
	if (MakeContextCurrent) glfwMakeContextCurrent(Window);
	glewInit();
	glViewport(0, 0, Width, Height);
	glfwSetFramebufferSizeCallback(Window, WindowSizeChanged);
	glfwSetCursorPosCallback(Window, MouseMoved);
	glEnable(GL_DEPTH_TEST);
	if (!InMenu)
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	ImGUIManager.InitializeImGUI(Window);
	return true;
}

bool CreateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// colour coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return true;
}

unsigned int CreateShader(const char* Source, int Type = GL_VERTEX_SHADER)
{
	unsigned int NewShader = glCreateShader(Type);
	glShaderSource(NewShader, 1, &Source, NULL);
	glCompileShader(NewShader);

	int Success;
	char InfoLog[512];

	glGetShaderiv(NewShader, GL_COMPILE_STATUS, &Success);

	if (!Success)
	{
		glGetShaderInfoLog(NewShader, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << InfoLog << std::endl;
	}

	return NewShader;
}

unsigned int CreateShaderProgram(std::vector<unsigned int> Shaders)
{
	unsigned int NewShaderProgram = glCreateProgram();
	for (unsigned int Shader : Shaders)
		glAttachShader(NewShaderProgram, Shader);
	glLinkProgram(NewShaderProgram);

	return NewShaderProgram;
}

float CameraSpeed = 0.05f;
void HandleInput()
{
	if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS)
	{
		WireframeMode = !WireframeMode;
		glPolygonMode(GL_FRONT_AND_BACK, WireframeMode ? GL_LINE : GL_FILL);
	}

	CameraSpeed = 0.0125f * MS;

	if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
		CameraPosition += CameraSpeed * CameraFront;
	if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
		CameraPosition -= CameraSpeed * CameraFront;
	if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
		CameraPosition -= glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
	if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
		CameraPosition += glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;

	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		InMenu = !InMenu;
		glfwSetInputMode(Window, GLFW_CURSOR, InMenu ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
}

void CheckAdjacentBlocks(const std::vector<std::vector<std::vector<Block>>>& Blocks, glm::vec3 Position, Block* block) {
	int WorldWidth = Blocks.size();
	int WorldHeight = Blocks[0].size();
	int WorldDepth = Blocks[0][0].size();

	block->DiscardTop = block->DiscardBottom = block->DiscardFront = block->DiscardBack = block->DiscardLeft = block->DiscardRight = false;

	// Check the right side
	if ((Position.x + 1) < (WorldWidth - 1) && Blocks[Position.x + 1][Position.y][Position.z].Base.Catagory != BlockCatagory::Air)
		block->DiscardRight = true;

	// Check the left side
	if ((Position.x - 1) >= 0 && Blocks[Position.x - 1][Position.y][Position.z].Base.Catagory != BlockCatagory::Air)
		block->DiscardLeft = true;

	// Check the top side
	if ((Position.y + 1) < WorldHeight && Blocks[Position.x][Position.y + 1][Position.z].Base.Catagory != BlockCatagory::Air)
		block->DiscardTop = true;

	// Check the bottom side
	if ((Position.y - 1) >= 0 && Blocks[Position.x][Position.y - 1][Position.z].Base.Catagory != BlockCatagory::Air)
		block->DiscardBottom = true;

	// Check the front side
	if ((Position.z + 1) < (WorldDepth - 1) && Blocks[Position.x][Position.y][Position.z + 1].Base.Catagory != BlockCatagory::Air)
		block->DiscardFront = true;

	// Check the back side
	if ((Position.z - 1) >= 0 && Blocks[Position.x][Position.y][Position.z - 1].Base.Catagory != BlockCatagory::Air)
		block->DiscardBack = true;
}


glm::vec2 GetChunkCoordFromVector3(glm::vec3 Position) {

	int X = floor(Position.x / 16), Y = floor(Position.z / 16);
	return glm::vec2(X, Y);
}

noise Noise;
BlockBase GrassBlock = BlockBase();
BlockBase DirtBlock = BlockBase();
BlockBase CobblestoneBlock = BlockBase();
BlockBase AirBlock = BlockBase();

void CreateChunk(int ChunkX, int ChunkY, int Width = 16, int Height = 16, int Depth = 16)
{
	std::cout << std::format("Creating Chunk: X{} Y{}\n", ChunkX, ChunkY);

	auto ChunkInfo = Noise.GenerateChunk(Noise.Perlin, ChunkX, ChunkY);
	std::vector<std::vector<std::vector<Block>>> Blocks(Width, std::vector<std::vector<Block>>(Height, std::vector<Block>(Depth)));

	int BaseX = ChunkX * 15;
	int BaseY = ChunkY * 15;
	Block NewBlock;

	#pragma omp parallel for collapse(3)
	for (int X = 0; X < Width; X++) {
		for (int Z = 0; Z < Depth; Z++) {
			int ColumnHeight = static_cast<int>((ChunkInfo[X][Z] + 1) * 0.5 * Height);

			for (int Y = 0; Y < Height; ++Y) {

				if (Y > ColumnHeight) {
					NewBlock.Base = AirBlock; // Above the height determined by noise
				}
				else if (Y == ColumnHeight) {
					NewBlock.Base = GrassBlock; // Top block at this height
				}
				else if (Y <= 1) {
					NewBlock.Base = CobblestoneBlock; // Bottom 2 blocks
				}
				else {
					NewBlock.Base = DirtBlock; // All other blocks below the top
				}

				NewBlock.Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(BaseX + X, Y, BaseY + Z));
				Blocks[X][Y][Z] = NewBlock;
			}
		}
	}

	ExistingChunks[std::make_pair(ChunkX, ChunkY)] = Blocks;
}

void CheckAdjacentBlocksForAllChunks()
{
	for (auto& [ChunkCoord, Blocks] : ExistingChunks)
	{
		int ChunkX = ChunkCoord.first;
		int ChunkY = ChunkCoord.second;
		chunkData& ChunkData = ExistingChunks[ChunkCoord];

		#pragma omp parallel for collapse(3)
		for (int X = 0; X < 15; X++) {
			std::vector<std::vector<Block>>& XBlocks = ChunkData[X];
			for (int Y = 0; Y < 15; Y++) {
				std::vector<Block>& YBlocks = XBlocks[Y];
				for (int Z = 0; Z < 15; Z++) {
					if (YBlocks[Z].Base.Catagory != BlockCatagory::Air)
						CheckAdjacentBlocks(Blocks, glm::vec3(X, Y, Z), &YBlocks[Z]);
				}
			}
		}
	}
}

int main()
{
	if (!InitializeGL() || !MakeWindow())
		return ExitCodes::Error;

	VertexShader = CreateShader(StandardVertexShaderSource, GL_VERTEX_SHADER);
	FragmentShader = CreateShader(StandardFragmentShaderSource, GL_FRAGMENT_SHADER);
	ShaderProgram = CreateShaderProgram(std::vector<unsigned int> { VertexShader, FragmentShader });

	CreateBuffers();

	Texture GrassTop = Texture("GrassTop.png");
	TextureMap["GrassTop"] = GrassTop.Binding;

	Texture Dirt = Texture("Dirt.png");
	TextureMap["Dirt"] = Dirt.Binding;

	Texture GrassSide = Texture("GrassSide.png");
	TextureMap["GrassSide"] = GrassSide.Binding;

	Texture Cobblestone = Texture("Cobblestone.png");
	TextureMap["Cobblestone"] = Cobblestone.Binding;

	TextureBinding GrassBindings[3] = { GrassTop.Binding, Dirt.Binding, GrassSide.Binding };
	GrassBlock.FillTexturesStandardBlock(GrassBindings);
	GrassBlock.Name = "Grass";

	TextureBinding DirtBindings[3] = { Dirt.Binding, Dirt.Binding, Dirt.Binding };
	DirtBlock.FillTexturesStandardBlock(DirtBindings);
	DirtBlock.Name = "Dirt";

	TextureBinding CobblestoneBindings[3] = { Cobblestone.Binding, Cobblestone.Binding, Cobblestone.Binding };
	CobblestoneBlock.FillTexturesStandardBlock(CobblestoneBindings);
	CobblestoneBlock.Name = "Cobblestone";

	BindAllTextures(ShaderProgram);

	AirBlock.Name = "Air";
	AirBlock.Catagory = BlockCatagory::Air;

	LastTime = glfwGetTime();
	glfwSwapInterval(0);
	glm::mat4 View = glm::mat4(1.0f);
	glm::mat4 Projection = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(Window))
	{
		double CurrentTime = glfwGetTime();
		Count++;
		
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		HandleInput();

		glUseProgram(ShaderProgram);

		View = glm::lookAt(CameraPosition, CameraPosition + CameraFront, CameraUp);
		Projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

		glUniformMatrix4fv(3, 1, GL_FALSE, &View[0][0]); // layout(location = 3) uniform mat4 View;
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(Projection)); // layout(location = 4) uniform mat4 Projection;

		bool ChunkChanged = false;
		glm::vec2 CameraChunkCoord = GetChunkCoordFromVector3(CameraPosition);
		const int RenderDistance = 2;
		#pragma omp parallel for collapse(2)
		for (int xOffset = -RenderDistance; xOffset <= RenderDistance; ++xOffset) {
			for (int yOffset = -RenderDistance; yOffset <= RenderDistance; ++yOffset) {
				glm::vec2 ChunkCoord = CameraChunkCoord + glm::vec2(xOffset, yOffset);
				auto ChunkCoordPair = std::make_pair(static_cast<int>(ChunkCoord.x), static_cast<int>(ChunkCoord.y));
				if (ExistingChunks.find(ChunkCoordPair) == ExistingChunks.end())
				{
					CreateChunk(ChunkCoord.x, ChunkCoord.y);
					ChunkChanged = true;
				}
			}
		}

		if (ChunkChanged)
			CheckAdjacentBlocksForAllChunks();

		for (auto& [ChunkCoord, Blocks] : ExistingChunks)
		{
			int ChunkX = ChunkCoord.first;
			int ChunkY = ChunkCoord.second;
			if (glm::distance(glm::vec2(ChunkX, ChunkY), CameraChunkCoord) > RenderDistance) continue;

			chunkData& Chunk = ExistingChunks[ChunkCoord];
			for (unsigned int X = 0; X < 15; X++) {
				std::vector<std::vector<Block>>& XBlocks = Chunk[X];
				for (unsigned int Y = 0; Y < 15; Y++) {
					std::vector<Block>& YBlocks = XBlocks[Y];
					for (unsigned int Z = 0; Z < 15; Z++) {
						Block* ThisBlock = &YBlocks[Z];
						if (ThisBlock->Base.Catagory == BlockCatagory::Air) continue;

						glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(ThisBlock->Matrix)); // layout(location = 2) uniform mat4 Model
						glUniform1i(106, // layout(location = 106) uniform int Discards;
							(ThisBlock->DiscardTop ? 1 : 0) |
							(ThisBlock->DiscardBottom ? 2 : 0) |
							(ThisBlock->DiscardFront ? 4 : 0) |
							(ThisBlock->DiscardBack ? 8 : 0) |
							(ThisBlock->DiscardLeft ? 16 : 0) |
							(ThisBlock->DiscardRight ? 32 : 0));

						ThisBlock->Base.SetShaderTextures(ShaderProgram);
						glDrawArrays(GL_TRIANGLES, 0, 36);
					}
				}
			}
		}

		ImGUIManager.StartFrame();

		ImGui::Begin("Minecraft | Statistics");

		if (CurrentTime - LastTime >= 0.1)
		{
			FPS = Count / (CurrentTime - LastTime);
			MS = (1.0 / FPS) * 1000;
			FPS = std::round(FPS * 10) / 10;
			MS = std::round(MS * 10) / 10;
			Count = 0;
			LastTime = CurrentTime;
		}

		ImGui::Text(std::format("Camera Chunk Coordinate: X{} Y{}", CameraChunkCoord.x, CameraChunkCoord.y).c_str());
		ImGui::Text(std::format("{} FPS", FPS).c_str());
		ImGui::Text(std::format("{} MS", MS).c_str());
		ImGui::End();

		ImGUIManager.EndFrame();
		glfwSwapBuffers(Window);

		glfwPollEvents();
	}

	ImGUIManager.StopImGUI();
	glfwTerminate();
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	return ExitCodes::Success;
}
