using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using XWEngine;

namespace XTextureizer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            XWAsyncFile.InitModule();
            m_rTexture = null;
            m_bTextureDirty = false;
            m_rBitmap = null;
            saveToolStripMenuItem.Enabled = false;
        }

        ~Form1()
        {
            XWAsyncFile.DeinitModule();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                m_rBitmap = new Bitmap(openFileDialog1.FileName);
                m_bTextureDirty = true;
                panel1.Invalidate();
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveFileDialog1.ShowDialog();

            // If the file name is not an empty string open it for saving.
            if(saveFileDialog1.FileName != "")
            {
                XWAsyncFile rFile = new XWAsyncFile();
                rFile.Open(saveFileDialog1.FileName, XWFileFlags.Write);
                byte[] rData = m_rTexture.Serialize();
                XWHandle hTransfer = rFile.BeginAsyncWrite(0, (uint)rData.Length, rData);
                uint nTransferred = rFile.FinishAsyncTransfer(hTransfer);
                rFile.Close();
            }
        }

        private void panel1_HandleCreated(object sender, EventArgs e)
        {
            m_rGraphics = new XWGraphics();
            m_rGraphics.Init();
            m_rGraphics.SetClearColor(new XWColor(1.0f, 1.0f, 1.0f, 1.0f));
        }

        private void panel1_HandleDestroyed(object sender, EventArgs e)
        {
            if (m_rTexture != null)
            {
                m_rTexture.Deinit(m_rGraphics);
                m_rTexture = null;
            }
            m_rGraphics.Deinit();
            m_rGraphics = null;
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            if (m_bTextureDirty)
            {
                if (m_rTexture != null)
                {
                    m_rTexture.Deinit(m_rGraphics);
                    m_rTexture = null;
                }
                m_rTexture = new XWTexture();
                m_rTexture.Init(m_rGraphics, XWTextureFlags.None, m_rBitmap);

                saveToolStripMenuItem.Enabled = true;
            }

            m_rGraphics.ClearScreen(XWScreenClearTarget.Color | XWScreenClearTarget.Depth);

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
                vertices);

            m_rGraphics.SetColor(new XWColor(0.0f, 0.0f, 0.0f, 1.0f));
            m_rGraphics.DrawElements(
                XWIndexType.U16,
                XWPrimitiveType.TriangleList,
                3,
                0,
                indices);

            m_rGraphics.PopRenderState();
        }

        Bitmap      m_rBitmap;
        bool        m_bTextureDirty;
        XWGraphics  m_rGraphics;
        XWTexture   m_rTexture;

    }
}