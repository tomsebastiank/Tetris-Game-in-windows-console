#include <iostream>
#include<Windows.h>
#include<thread>
#include<vector>
using namespace std;

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
int nScreenWidth = 80;
int nScreenHeight = 30;
int nScore = 0;
//This is not assigned yet this will be full array
unsigned char * pField = nullptr;

int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{
		case 0: return py * 4 + px;
		case 1: return 12 + py - ( px * 4 );
		case 2: return 15 - (py * 4)- px;
		case 3: return 3 - py + (px * 4);
	}
	return 0;
}


bool DoesPieceFit(int nTetromino, int nRotation, int PosX, int PosY)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{
			int pi = Rotate(px, py, nRotation);

			int fi = (PosY + py)*nFieldWidth + (PosX + px);


			if (PosX + px >= 0 && PosX + px < nFieldWidth)
			{
				if (PosY + py >= 0 && PosY + py < nFieldHeight)
				{
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false;
				}
			}
		}
	}

	return true;

}
int main()
{
	//Create assets
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X."); 
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L".X..");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L"..X.");
	tetromino[1].append(L"....");

	tetromino[2].append(L"..X.");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"XX..");
	tetromino[3].append(L"X...");
	tetromino[3].append(L"X...");
	tetromino[3].append(L"X...");

	tetromino[4].append(L"X...");
	tetromino[4].append(L"X...");
	tetromino[4].append(L"X...");
	tetromino[4].append(L"XX..");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"....");

	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L"....");

	pField = new unsigned char[nFieldHeight* nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;


	bool bGameOver = false;
	bool bKey[4];
	bool bRotateHeld = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = 3;
	int nCurrentY = 0;
	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForcedown = false;
	vector<int>Lines;

	while (!bGameOver)
	{ 

	 
		//GAME TIMING
		this_thread::sleep_for(50ms);
		//Sleep(50);
		nSpeedCounter++;
		bForcedown = (nSpeedCounter == nSpeed);


		// Input ========================
		for (int k = 0; k < 4; k++)								// R   L   D Z(Rotation)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		
		nCurrentX += bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY) ? 1 : 0;
		nCurrentX -= bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY) ? 1 : 0;
		nCurrentY += bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1) ? 1 : 0;
		

		if (bKey[3])
		{
			nCurrentRotation += !bRotateHeld && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY) ? 1 : 0;
			bRotateHeld = true;
		}
		else
			bRotateHeld = false;

		//LOGIC

		

		if (bForcedown)
		{
			if(DoesPieceFit(nCurrentPiece, nCurrentRotation , nCurrentX, nCurrentY+1))
				nCurrentY++;			
			else
			{
				//Lock the current piece in the field
				for (int x = 0; x < 4; x++)
					for (int y = 0; y < 4; y++)
						if (tetromino[nCurrentPiece][Rotate(x, y, nCurrentRotation)] == L'X')
							pField[(nCurrentY + y)*nFieldWidth + (nCurrentX + x)] = nCurrentPiece + 1;
				// Different tertomenos will have different id .. 


				// check we got a line
				// we need to check the last 4 lines  only 
				//takes the four rows
				for (int py = 0; py < 4; py++)
				{
					//consider the border for FieldHeight
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						//consider the border for FieldWidth
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py)*nFieldWidth + px] )!= 0;

						//if bline = true 
						// we will dispaly equal =============
						if (bLine)
						{

							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py)*nFieldWidth + px] = 8;

							Lines.push_back(nCurrentY + py);
						}



					}
				}

				nScore += 25;
				if (!Lines.empty())	nScore += (1 << Lines.size()) * 100;

				//next piece
				nCurrentX = 3;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				//if the new piec not fit boooooom Game over
				bGameOver = !(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY));
			}
			
			nSpeedCounter = 0;

		}
		

		//RENDER
		//Draw Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2)*nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];
			
	
		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;


		// Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
		//Display Frame

		if (!Lines.empty())
		{
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms);
			//sometimes there may be more than one lines
			for (auto &v : Lines)
			{
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
					{
						pField[py*nFieldWidth + px] = pField[(py - 1)*nFieldWidth + px];
					}
					pField[0 + px] = 0;

				}

			}
			Lines.clear();
		}
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	// Oh Dear
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << nScore << endl;
	system("pause");
	return 0;
	





	
}


