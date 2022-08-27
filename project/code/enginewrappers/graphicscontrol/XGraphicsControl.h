#pragma once
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#include <GL/gl.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace XWEngine {

	/// <summary>
	/// Summary for XGraphicsControl
	/// </summary>
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	public ref class XGraphicsControl : public System::Windows::Forms::UserControl
	{
	public:
		XGraphicsControl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~XGraphicsControl()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// XGraphicsControl
			// 
			this->BackColor = System::Drawing::SystemColors::Desktop;
			this->ResumeLayout(false);

		}
#pragma endregion
	protected: 
		virtual void OnHandleCreated(EventArgs^ e) override;
		virtual void OnHandleDestroyed(EventArgs^ e) override;
		virtual void OnPaint( PaintEventArgs^ e ) override;
		virtual void OnResize(EventArgs^ e) override;

	private:
		HWND	m_hWnd;
		HDC		m_hDC;
		HGLRC	m_hGLRC;

	};
}
