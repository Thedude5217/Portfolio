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
    public partial class RandommodalDialogue : Form
    {
        //creating the variable I need
       public NumericUpDown steve;
        public RandommodalDialogue()
        {
            //I named the variable steve because I was very tired and it was the only thing I could think of
            //This sets the variable equal to the value of the numericupdown
            InitializeComponent();
            steve = numericUpDown1;
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
        //This sets the numericupdown value to whatever the user inputs
        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            decimal uservalue = numericUpDown1.Value;
            Random seed = new Random(Decimal.ToInt32(uservalue));
            
           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

       
    }
}
