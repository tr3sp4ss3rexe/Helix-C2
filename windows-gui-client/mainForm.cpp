#include "mainForm.h"

using namespace System;
using namespace System::Windows::Forms;

int main() {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Helix::mainForm form;
	Application::Run(% form);


	return 0; 
}