using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using XWEngine;
using DataTreeTools;

namespace datatreefarm
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            m_rFactory = new CWriteableDataTreeFactory();
            m_rDataTree = null;
            m_rAllocator = new XWAllocator();
            saveToolStripMenuItem.Enabled = false;
        }

        ~Form1()
        {
        }


        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                System.IO.TextReader rReader = new System.IO.StreamReader(openFileDialog1.FileName);
                if (openFileDialog1.FilterIndex == 1) ///First filter index is 1.  WTFBBQ
                {
                    unsafe
                    {
                        m_rDataTree = new CWriteableDataTreeWrapper(m_rFactory.CreateFromCSV(rReader, m_rAllocator));
                    }
                }
                else
                {
                    unsafe
                    {
                        m_rDataTree = new CWriteableDataTreeWrapper(m_rFactory.CreateFromXML(rReader, m_rAllocator));
                    }
                }
                if (m_rDataTree != null)
                {
                    saveToolStripMenuItem.Enabled = true;
                }
            }
        }


        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveFileDialog1.ShowDialog();

            // If the file name is not an empty string open it for saving.
            if (saveFileDialog1.FileName != "")
            {
                XWSyncFile rFile = new XWSyncFile();
                rFile.Open(saveFileDialog1.FileName, XWFileFlags.Write);
                XWEngine.XWTools.XWResourceDatabaseBuilder rDBBuilder = new XWEngine.XWTools.XWResourceDatabaseBuilder();
                rDBBuilder.AddObject(m_rDataTree);
                rDBBuilder.XWFinalize(rFile, true);

//                 byte[] rData = m_rTexture.Serialize();
//                 XWHandle hTransfer = rFile.BeginAsyncWrite(0, (uint)rData.Length, rData);
//                 uint nTransferred = rFile.FinishAsyncTransfer(hTransfer);
                 rFile.Close();
            }
        }


        private CWriteableDataTreeFactory   m_rFactory;
        private XWAllocator                 m_rAllocator;
        private CWriteableDataTreeWrapper   m_rDataTree; 
    }
}