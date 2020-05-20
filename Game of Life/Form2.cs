using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Game_of_Life
{
    public partial class Form2 : Form
    {
        //creating the variables I need
        public NumericUpDown miliseconds;
        public NumericUpDown x;
        public NumericUpDown y;
        public Form2()
        {
            //This sets the variables equal to the value of the numericupdown
            InitializeComponent();
            miliseconds = numericUpDown1;
            x = numericUpDown2;
            y = numericUpDown3;
        }
        //These set the numericupdown values to whatever the user inputs
        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            decimal uservalue1 = numericUpDown1.Value;
        }

        private void numericUpDown2_ValueChanged(object sender, EventArgs e)
        {
            decimal uservalue2 = numericUpDown2.Value;
        }

        private void numericUpDown3_ValueChanged(object sender, EventArgs e)
        {
            decimal uservalue3 = numericUpDown3.Value;
        }
        private void Form2_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {

        }
    }
}
