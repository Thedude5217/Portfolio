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

namespace Game_of_Life
{
    public partial class Form1 : Form
    {
        // The universe array
        bool[,] universe = new bool[20, 20];
        bool[,] ScratchPad = new bool[20, 20];
        // Drawing colors
        Color gridColor = Color.Black;
        Color cellColor = Color.Gray;
        bool view = true;
        bool display = true;
        // The Timer class
        Timer timer = new Timer();

       
        int currentSeed = 0;
        // Generation count
        int generations = 0;
        //int cells = 0;


        public Form1()
        {
            InitializeComponent();

            graphicsPanel1.BackColor = Properties.Settings.Default.DeadColor;
            cellColor = Properties.Settings.Default.AliveColor;

            // Setup the timer
            timer.Interval = 100; // milliseconds
            timer.Tick += Timer_Tick;
            timer.Enabled = false; // start timer running
        }

        private int Neighbor(int x, int y)
        {
            int neighbors = 0;
            int mx = 0;
            int my = 0;
           for (int i = - 1; i < 2; i++)
           {
               for (int j = - 1; j < 2; j++)
               {
                    mx = x + j;
                    my = y + i;
                   if (mx < 0 || mx > universe.GetLength(0) - 1|| my < 0 || my > universe.GetLength(1) - 1)
                   {
                        break;
                                
                   }
                   if (universe[mx,my])
                   {
                       neighbors++;
                   }

                }

           }
            if (!universe[x,y])
            {

            return neighbors;
            }
            return neighbors - 1;
        }
       //This is where I implement the rules
        private void Rules()
        {
            
            int count = 0;
            for (int y = 0; y < universe.GetLength(1); y++)
            {
                
                for (int x = 0; x < universe.GetLength(0); x++)
                {
                    count = Neighbor(x, y);
                    if (universe[x, y])
                    {
                        if (count < 2 || count > 3)
                        {
                           ScratchPad[x, y] = false;
                        }
                        if (count == 2 || count == 3)
                        {
                          ScratchPad[x, y] = true;
                        }

                    }
                    else
                    {
                         if (count == 3)
                        {

                            ScratchPad[x, y] = true;
                        }
                         else
                        {
                            ScratchPad[x, y] = false;
                        }

                    }
                   
                }
            }
            graphicsPanel1.Invalidate();
           
        }
        private void update()
        {
            bool[,] temp = universe;
            universe = ScratchPad;
            ScratchPad = temp;
        }
        // Calculate the next generation of cells
        int LivingCells()
        {
            int alive = 0;
            for (int y = 0; y < universe.GetLength(1); y++)
            {
                // Iterate through the universe in the x, left to right
                for (int x = 0; x < universe.GetLength(0); x++)
                {
                    if (universe[x, y] == true)
                    {
                        alive++;
                    }
                }
            }
            return alive;
        }
        private void NextGeneration()
        {
           

            Rules();
            // Increment generation count
            generations++;
            // Update status strip generations
            toolStripStatusLabelGenerations.Text = "Generations = " + generations.ToString();
            
            toolStripProgressBar1.Text = "Living Cells = " + LivingCells();
            graphicsPanel1.Invalidate();
            update();
        }

        // The event called by the timer every Interval milliseconds.
        private void Timer_Tick(object sender, EventArgs e)
        {
            NextGeneration();
        }
        //this is to display the grid and the neighbor count inside of the cells
        private void graphicsPanel1_Paint(object sender, PaintEventArgs e)
        {
                // Calculate the width and height of each cell in pixels
                // CELL WIDTH = WINDOW WIDTH / NUMBER OF CELLS IN X
                int cellWidth = graphicsPanel1.ClientSize.Width / universe.GetLength(0);
                // CELL HEIGHT = WINDOW HEIGHT / NUMBER OF CELLS IN Y
                int cellHeight = graphicsPanel1.ClientSize.Height / universe.GetLength(1);

                // A Pen for drawing the grid lines (color, width)
                Pen gridPen = new Pen(gridColor, 1);

                // A Brush for filling living cells interiors (color)
                Brush cellBrush = new SolidBrush(cellColor);

                // Iterate through the universe in the y, top to bottom
                for (int y = 0; y < universe.GetLength(1); y++)
                {
                    // Iterate through the universe in the x, left to right
                    for (int x = 0; x < universe.GetLength(0); x++)
                    {
                        if (view == true)
                        {
                            // A rectangle to represent each cell in pixels
                            RectangleF cellRect = RectangleF.Empty;
                            cellRect.X = x * cellWidth;
                            cellRect.Y = y * cellHeight;
                            cellRect.Width = cellWidth;
                            cellRect.Height = cellHeight;

                            // Fill the cell with a brush if alive
                            if (universe[x, y] == true)
                            {
                                e.Graphics.FillRectangle(cellBrush, cellRect);
                            }

                            if (display == true)
                            {
                                Font font = new Font("Arial", 20f);

                                StringFormat stringFormat = new StringFormat();
                                stringFormat.Alignment = StringAlignment.Center;
                                stringFormat.LineAlignment = StringAlignment.Center;

                                Rectangle rect = new Rectangle(0, 0, 100, 100);
                    
                                //Neighbor(x,y);
                                if (Neighbor(x,y) > 0 || universe[x,y])
                                {

                                e.Graphics.DrawString(Neighbor(x,y).ToString(), font, Brushes.Black, cellRect, stringFormat);
                                }
                            }

                                // Outline the cell with a pen
                                e.Graphics.DrawRectangle(gridPen, cellRect.X, cellRect.Y, cellRect.Width, cellRect.Height);
                        }
                    }
                }


                // Cleaning up pens and brushes
                gridPen.Dispose();
                cellBrush.Dispose();
        }

        private void graphicsPanel1_MouseClick(object sender, MouseEventArgs e)
        {
            // If the left mouse button was clicked
            if (e.Button == MouseButtons.Left)
            {
                // Calculate the width and height of each cell in pixels
                int cellWidth = graphicsPanel1.ClientSize.Width / universe.GetLength(0);
                int cellHeight = graphicsPanel1.ClientSize.Height / universe.GetLength(1);

                // Calculate the cell that was clicked in
                // CELL X = MOUSE X / CELL WIDTH
                int x = e.X / cellWidth;
                // CELL Y = MOUSE Y / CELL HEIGHT
                int y = e.Y / cellHeight;

                // Toggle the cell's state
                universe[x, y] = !universe[x, y];

                // Tell Windows you need to repaint
                graphicsPanel1.Invalidate();
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Iterate through the universe in the y, top to bottom
            for (int y = 0; y < universe.GetLength(1); y++)
            {
                // Iterate through the universe in the x, left to right
                for (int x = 0; x < universe.GetLength(0); x++)
                {
                    universe[x, y] = false;
                }
            }
            generations = 0;
            graphicsPanel1.Invalidate();
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            timer.Enabled = true;
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            timer.Enabled = false;
        }

        private void toolStripButton3_Click(object sender, EventArgs e)
        {
            NextGeneration();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
        //This is so the user can save the world to a file
        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
                SaveFileDialog dlg = new SaveFileDialog();
                dlg.Filter = "All Files|*.*|Cells|*.cells";
                dlg.FilterIndex = 2; dlg.DefaultExt = "cells";


                if (DialogResult.OK == dlg.ShowDialog())
                {
                    StreamWriter writer = new StreamWriter(dlg.FileName);

                    // Write any comments you want to include first.
                    // Prefix all comment strings with an exclamation point.
                    // Use WriteLine to write the strings to the file. 
                    // It appends a CRLF for you.
                    //writer.WriteLine("!This is my comment.");

                    // Iterate through the universe one row at a time.
                    for (int y = 0; y < universe.GetLength(1); y++)
                    {
                        // Create a string to represent the current row.
                        String currentRow = string.Empty;

                        // Iterate through the current row one cell at a time.
                        for (int x = 0; x < universe.GetLength(0); x++)
                        {
                            // If the universe[x,y] is alive then append 'O' (capital O)
                            // to the row string.
                            if (universe[x, y] == true)
                            {
                            //using (writer = File.AppendText(dlg.FileName))
                            //{
                                currentRow += "O";
                                //}
                            }

                            // Else if the universe[x,y] is dead then append '.' (period)
                            // to the row string.
                            else if (universe[x, y] == false)
                            {
                                //using (writer = File.AppendText(dlg.FileName))
                                //{
                                   currentRow += ".";
                               // }
                            }
                        }
                      writer.WriteLine(currentRow);
                        // Once the current row has been read through and the 
                        // string constructed then write it to the file using WriteLine.
                    }

                    // After all rows and columns have been written then close the file.
                    writer.Close();
                }
        }
        //This is to open an existing universe
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "All Files|*.*|Cells|*.cells";
            dlg.FilterIndex = 2;

            if (DialogResult.OK == dlg.ShowDialog())
            {
                StreamReader reader = new StreamReader(dlg.FileName);

                // Create a couple variables to calculate the width and height
                // of the data in the file.
                int cellWidth = 0;
                int cellHeight = 0;

                // Iterate through the file once to get its size.
                while (!reader.EndOfStream)
                {
                    // Read one row at a time.
                    string row = reader.ReadLine();

                    // If the row begins with '!' then it is a comment
                    // and should be ignored.
                    if (row.StartsWith("!"))
                    {
                        continue;
                    }

                    // If the row is not a comment then it is a row of cells.
                    // Increment the maxHeight variable for each row read.
                    // Get the length of the current row string
                    // and adjust the maxWidth variable if necessary.
                    else if (!row.StartsWith("!"))
                    {
                        cellHeight++;
                        cellWidth = row.Length;
                    }

                }

                // Resize the current universe and scratchPad
                // to the width and height of the file calculated above.
                universe = new bool[cellWidth,cellHeight];
                ScratchPad = new bool[cellWidth, cellHeight];
                // Reset the file pointer back to the beginning of the file.
                reader.BaseStream.Seek(0, SeekOrigin.Begin);

                int yPos = 0;
                // Iterate through the file again, this time reading in the cells.
                while (!reader.EndOfStream)
                {
                    // Read one row at a time.
                    string row = reader.ReadLine();

                    // If the row begins with '!' then
                    // it is a comment and should be ignored.
                    if (row[0] == '!')
                    {
                        continue;
                    }
                    // If the row is not a comment then 
                    // it is a row of cells and needs to be iterated through.
                    for (int xPos = 0; xPos < row.Length; xPos++)
                    {
                        // If row[xPos] is a 'O' (capital O) then
                        // set the corresponding cell in the universe to alive.
                        if (row[xPos] == 'O')
                        {
                            universe[xPos, yPos] = true;
                        }
                        // If row[xPos] is a '.' (period) then
                        // set the corresponding cell in the universe to dead.
                        else if(row[xPos] == '.')
                        {
                            universe[xPos, yPos] = false;
                        }
                    }

                    yPos++;
                }

                // Close the file.
                reader.Close();
            }

            graphicsPanel1.Invalidate();
        }

        private void toolStripProgressBar1_Click(object sender, EventArgs e)
        {
            
        }
        //Randomization by Time
        private void byTimeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Random time = new Random();
            for (int y = 0; y < universe.GetLength(1); y++)
            {
                for (int x = 0; x < universe.GetLength(0); x++)
                {
                    int Time = time.Next(0, 3);
                    if (Time == 0)
                    {
                        universe[x, y] = true;
                    }
                    else
                    {
                        universe[x, y] = false;
                    }
                }
            }
        }
        //Randomization by Seed
        private void bySeedToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RandommodalDialogue modal = new RandommodalDialogue();
            modal.steve.Value = currentSeed;
            if (DialogResult.OK == modal.ShowDialog())
            {
                currentSeed = (int)modal.steve.Value;
                Randomize(currentSeed);
            }
        }
        private void Randomize(int _currentSeed)
        {
            Random time = new Random(currentSeed);
            for (int y = 0; y < universe.GetLength(1); y++)
            {
                for (int x = 0; x < universe.GetLength(0); x++)
                {
                    int Time = time.Next(0, 3);
                    if (Time == 0)
                    {
                        universe[x, y] = true;
                    }
                    else
                    {
                        universe[x, y] = false;
                    }
                }
            }
        }
        //Changing the miliseconds and the universe size
        //The universe has trouble rezizing sometimes, I asked the lab techs and even they can't figure out why
        //If you try resizing twice, or hitting the next generation, it should work
        private void changeTimeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int universex = universe.GetLength(0);
            int universey = universe.GetLength(1);
            Form2 time = new Form2();
            //Form2 changex = new Form2();
            //Form2 changey = new Form2();
            time.miliseconds.Value = timer.Interval;
            time.x.Value = universex;
            time.y.Value = universey;
            if (DialogResult.OK == time.ShowDialog())
            {
                if (generations == 0)
                {
                    timer.Interval = (int)time.miliseconds.Value;
                    universex = (int)time.x.Value;
                    universey = (int)time.y.Value;

                    bool[,] nsize = new bool[universex, universey];
                    bool[,] temp = nsize;
                    nsize = universe;
                    universe = ScratchPad;
                    ScratchPad = temp;
                }
            }
            graphicsPanel1.Invalidate();
        }

        private void changeSizeToolStripMenuItem_Click(object sender, EventArgs e)
        {
           
        }

        private void viewToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
        }
        //toggling the grid
        private void toggleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (view == true)
            {
                view = false;
            }
            else if (view == false)
            {
                view = true;
            }
            graphicsPanel1.Invalidate();
        }
        //toggling the neighbor count display inside the cells
        private void toggleNeighborCountToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (display == true)
            {
                display = false;
            }
            else if (display == false)
            {
                display = true;
            }
            graphicsPanel1.Invalidate();
        }
        //this is for the cell color
        private void aliveColorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ColorDialog alive = new ColorDialog();
            alive.Color = cellColor;
            if (DialogResult.OK == alive.ShowDialog())
            {
                cellColor = alive.Color;
            }
            graphicsPanel1.Invalidate();
        }
        //setting the universe color
        private void deadColorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ColorDialog dead = new ColorDialog();
            dead.Color = graphicsPanel1.BackColor;
            if (DialogResult.OK == dead.ShowDialog())
            {
                graphicsPanel1.BackColor = dead.Color;
            }
            graphicsPanel1.Invalidate();
        }
        //This saves the colors chosen
        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            Properties.Settings.Default.AliveColor = cellColor;
            Properties.Settings.Default.DeadColor = graphicsPanel1.BackColor;
            Properties.Settings.Default.Save();
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {

        }

        private void colorToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
    }
}
