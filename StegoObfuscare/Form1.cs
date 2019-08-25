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
            Form11 f = new Form11();
            f.Show();
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
            Form5 f = new Form5();
            f.Show();
        }

        private void Button2_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\DataBase\\Grafic obfuscare.xlsx");
        }

        private void Button3_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\source\\Analizator\\analyzator.exe");
        }

        private void Button7_Click(object sender, EventArgs e)
        {
            Form6 f = new Form6();
            f.Show();
        }

        private void Button4_Click(object sender, EventArgs e)
        {

        }

        private void Button9_Click(object sender, EventArgs e)
        {
            Form16 f = new Form16();
            f.Show();
        }

        private void Button5_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\source\\TestPE-Editor\\test32.exe");
        }

        private void Button8_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\source\\TestDictionary\\add_synonym32.exe");
        }

        private void Button10_Click(object sender, EventArgs e)
        {
            Process.Start("D:\\Project\\StegoObfuscareApp\\source\\TestDictionary\\print_synonym32.exe");
        }

        private void ToolStripMenuItem3_Click(object sender, EventArgs e)
        {
            Form2 f = new Form2();
            f.Show();
        }

        private void ВложениеВФункциюToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form3 f = new Form3();
            f.Show();
        }

        private void ВнедрениеКодаВМодулиToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form4 f = new Form4();
            f.Show();
        }

        private void ToolStripMenuItem4_Click(object sender, EventArgs e)
        {
            Form7 f = new Form7();
            f.Show();
        }

        private void ToolStripMenuItem5_Click(object sender, EventArgs e)
        {
            Form8 f = new Form8();
            f.Show();
        }

        private void ToolStripMenuItem6_Click(object sender, EventArgs e)
        {
            Form9 f = new Form9();
            f.Show();
        }

        private void ToolStripMenuItem9_Click(object sender, EventArgs e)
        {
            Form10 f = new Form10();
            f.Show();
        }

        private void ToolStripMenuItem11_Click(object sender, EventArgs e)
        {
            Form12 f = new Form12();
            f.Show();
        }

        private void ToolStripMenuItem12_Click(object sender, EventArgs e)
        {
            Form13 f = new Form13();
            f.Show();
        }

        private void ToolStripMenuItem14_Click(object sender, EventArgs e)
        {
            Form14 f = new Form14();
            f.Show();
        }

        private void ToolStripMenuItem15_Click(object sender, EventArgs e)
        {
            Form15 f = new Form15();
            f.Show();
        }
    }
}
