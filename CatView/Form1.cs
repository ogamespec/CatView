using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.IO;

namespace CatView
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void testHexViewToolStripMenuItem_Click(object sender, EventArgs e)
        {
            byte[] data = new byte[256];

            for(int i=0; i<data.Length; i++)
            {
                data[i] = (byte)i;
            }

            memoryViewControl1.Data = data;
            memoryViewControl1.Invalidate();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void loadCATFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if ( openFileDialog1.ShowDialog() == DialogResult.OK )
            {
                Cat cat = new Cat(openFileDialog1.FileName);
                PopulateTreeView(cat);
            }
        }

        private void PopulateTreeView (Cat cat)
        {
            treeView1.Nodes.Clear();

            treeView1.BeginUpdate();

            int index = 0;
            foreach (var file in cat.files)
            {
                TreeNode node = new TreeNode("Entry " + index.ToString() + ", hash: " + file.hash.ToString("X8"));

                node.Tag = file;

                treeView1.Nodes.Add(node);

                index++;
            }

            treeView1.EndUpdate();
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            TreeNode node = treeView1.SelectedNode;

            if (node != null)
            {
                CatFile file = (CatFile)node.Tag;

                memoryViewControl1.Data = file.data;
                memoryViewControl1.ScrollingOffset = 0;
                memoryViewControl1.Invalidate();
            }
        }

        private void saveSelectedAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if ( memoryViewControl1.Data != null)
                {
                    File.WriteAllBytes(saveFileDialog1.FileName, memoryViewControl1.Data);
                }
            }
        }
    }
}
