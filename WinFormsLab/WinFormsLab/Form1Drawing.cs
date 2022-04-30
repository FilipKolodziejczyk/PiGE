using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Globalization;
using System.Threading;
using System.Reflection;

namespace WinFormsLab
{
    public partial class Form1 : Form
    {
        private bool drawing = false;
        private Point previousPoint;
        private Color currentColor => choosenColorButton.BackColor;
        private Pen pen;
        private Image? originalImage;

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (selectedTool == null)
                return;

            if (e.Button == MouseButtons.Right)
            {
                if (selectedTool == rectangleButton || selectedTool == ellipseButton)
                {
                    pictureBox1.Image.Dispose();
                    pictureBox1.Image = new Bitmap(originalImage!);
                    pictureBox1.Refresh();
                }
                endDrawing();
            }
            else if (e.Button == MouseButtons.Left)
            {
                originalImage = new Bitmap(pictureBox1.Image);
                pen = new Pen(currentColor, (int)lineThickness);
                previousPoint = new Point(e.X, e.Y);
                drawing = true;
            }
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (selectedTool == null || drawing == false)
                return;

            if (e.Button == MouseButtons.Left)
                endDrawing();
        }

        private void endDrawing()
        {
            originalImage!.Dispose();
            originalImage = null;
            pen.Dispose();
            drawing = false;
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (drawing)
            {
                if (selectedTool == rectangleButton || selectedTool == ellipseButton)
                {
                    pictureBox1.Image.Dispose();
                    pictureBox1.Image = new Bitmap(originalImage!);
                }

                using (Graphics g = Graphics.FromImage(pictureBox1.Image))
                {
                    if (selectedTool == rectangleButton || selectedTool == ellipseButton)
                    {
                        if (selectedTool == rectangleButton)
                        {
                            int X = e.X < previousPoint.X ? e.X : previousPoint.X;
                            int Y = e.Y < previousPoint.Y ? e.Y : previousPoint.Y;
                            Point leftTop = new Point(X, Y);
                            Size size = new Size(Math.Abs(e.X - previousPoint.X), Math.Abs(e.Y - previousPoint.Y));
                            Rectangle rect = new Rectangle(leftTop, size);
                            g.DrawRectangle(pen, rect);
                        }
                        else if (selectedTool == ellipseButton)
                        {
                            Size size = new Size(e.X - previousPoint.X, e.Y - previousPoint.Y);
                            Rectangle rect = new Rectangle(previousPoint, size);
                            g.DrawEllipse(pen, rect);
                        }
                    }
                    else
                    {
                        g.DrawLine(pen, previousPoint, e.Location);
                        previousPoint = e.Location;
                    }
                }
                
                pictureBox1.Refresh();
            }
        }
    }
}
