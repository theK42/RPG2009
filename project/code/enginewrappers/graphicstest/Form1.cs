using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using XWEngine;
using XEngine;

namespace graphicstest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void panel1_HandleCreated(object sender, EventArgs e)
        {
            m_rGraphics = new XWGraphics();
            m_rGraphics.Init();
            m_rGraphics.SetClearColor(new XWColor(1.0f, 1.0f, 1.0f, 1.0f));
        }

        private void panel1_HandleDestroyed(object sender, EventArgs e)
        {
            m_rGraphics.Deinit();
            m_rGraphics = null;
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {

            m_rGraphics.ClearScreen( XWScreenClearTarget.Color | XWScreenClearTarget.Depth );

		    float[] vertices =
		    {
			    0, 0, 0 ,
			    1, 0, 0 ,
		    	1, 1, 0
		    };

     		ushort[] indices =
 	    	{
 		    	0, 1, 2
 	    	};
 
 		    m_rGraphics.PushRenderState();
 
 		    m_rGraphics.SetVertexAttributePointer(
 			    XWVertexAttributeId.Position,
 			    XWVertexAttributeType.F32,
 			    3,
 			    12, /// 3 * 4 bytes
 			    0,
 			    vertices );
 
 		    m_rGraphics.SetColor(new XWColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
 		    m_rGraphics.DrawElements(
 			    XWIndexType.U16,
 			    XWPrimitiveType.TriangleList,
 			    3,
 			    0,
 			    indices );
     
 		    m_rGraphics.PopRenderState();
        }

        private XWEngine.XWGraphics m_rGraphics;
    }
}