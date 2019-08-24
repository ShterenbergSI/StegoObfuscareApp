using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.OleDb;
using System.Windows.Forms;
using System.Diagnostics;

namespace StegoObfuscare
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void ToolStripMenuItem2_Click(object sender, EventArgs e)
        {

        }

        private void ToolStripMenuItem8_Click(object sender, EventArgs e)
        {

        }

        private void ToolStripMenuItem10_Click(object sender, EventArgs e)
        {

        }

        private void Button1_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\DataBase\\Grafic stego.xlsx");
        }

        private void Label1_Click(object sender, EventArgs e)
        {

        }

        private void Button6_Click(object sender, EventArgs e)
        {

        }

        private void Button2_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\DataBase\\Grafic obfuscare.xlsx");
        }

        private void Button3_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\source\\Analizator\\analyzator.exe");
        }
    }
}
