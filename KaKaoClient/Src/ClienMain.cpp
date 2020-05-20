#include "MainForm.h"

int main()
{
	MainForm mainform;
	mainform.Init();
	while (true)
		mainform.UpdateSceen();
}