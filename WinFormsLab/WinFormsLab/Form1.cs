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
        private const int colorButtonBorderWidth = 3;
        private const int colorButtonSize = 25;
        private (float Width, float Height) pictureToFormRatio;

        public Form1()
        {
            CultureInfo.CurrentUICulture = new CultureInfo("en", false);
            InitializeComponent();
            
            // Initial settings
            thicknessComboBox.SelectedIndex = (int)Thickness.Medium;
            Color initColor = Color.Black;
            selectedTool = null;

            // Saving ratio for later resizes
            pictureToFormRatio.Width = (float)this.Width / this.pictureBox1.Width;
            pictureToFormRatio.Height = (float)this.Height / this.pictureBox1.Height;

            Bitmap canva = new Bitmap(this.pictureBox1.Width, this.pictureBox1.Height);
            Graphics canvaGraphics = Graphics.FromImage(canva);
            canvaGraphics.Clear(Color.White);
            pictureBox1.Image = canva;

            // Color Buttons creation with choosing inital color
            loadColorButtons(initColor);
        }

        // Create color buttons based on KnownColor enumerator
        private void loadColorButtons(Color initColor)
        {
            foreach (KnownColor color in Enum.GetValues(typeof(KnownColor)))
            {
                Color backColor = Color.FromKnownColor(color);
                Color borderColor = Color.FromArgb(255 - backColor.R, 255 - backColor.G, 255 - backColor.B);

                Button colorButton = new Button
                {
                    Name = $"colorButton_{color}",
                    Width = colorButtonSize,
                    Height = colorButtonSize,
                    BackColor = backColor,
                    FlatStyle = FlatStyle.Flat,
                    FlatAppearance = { BorderColor = borderColor, BorderSize = 0 }
                };
                colorButton.Paint += new PaintEventHandler(colorButton_Paint);
                colorButton.Click += colorButton_Click;

                if (colorButton.BackColor == initColor)
                {
                    choosenColorButton = colorButton;
                    choosenColorBox.BackColor = initColor;
                }

                flowLayoutPanel1.Controls.Add(colorButton);
            }
        }

        // Simplification of complex method
        public static void DrawButtonBorder(Graphics graphics, Rectangle bounds, Color color, ButtonBorderStyle style, int width)
        {
            ControlPaint.DrawBorder(graphics, bounds, color, width, style, color, width, style, color, width, style, color, width, style);
        }

        // Draws button with dashed border if selected, wihtout any border otherwise
        private void colorButton_Paint(object sender, PaintEventArgs e)
        {
            Button button = (Button)sender;
            ButtonBorderStyle borderStyle;
            if (button == choosenColorButton)
                borderStyle = ButtonBorderStyle.Dashed;
            else
                borderStyle = ButtonBorderStyle.None;

            Rectangle borderRectanle = button.ClientRectangle;
            DrawButtonBorder(e.Graphics, borderRectanle, button.FlatAppearance.BorderColor, borderStyle, colorButtonBorderWidth);
        }


        // Resizes canva to match new size of pictureBox
        // Undistinguishable by user from SizeChanged (pictureBox has white background) but more efficient
        // It removes part of canva which will be out of form and fills new space with white color
        private void Form1_ResizeEnd(object sender, EventArgs e)
        {
            SuspendLayout();
            
            Bitmap canva = new Bitmap(this.pictureBox1.Width, this.pictureBox1.Height);
            Graphics canvaGraphics = Graphics.FromImage(canva);
            canvaGraphics.DrawImage(pictureBox1.Image, 0, 0);
            pictureBox1.Image.Dispose();
            pictureBox1.Image = canva;

            ResumeLayout();
        }

        // Used only for maximazing and exiting maximized mode (which doesn't trigger ResizeEnd)
        FormWindowState lastWindowState = FormWindowState.Normal;
        private void Form1_Resize(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Maximized || (WindowState == FormWindowState.Normal && lastWindowState == FormWindowState.Maximized))
                Form1_ResizeEnd(sender, e);

            lastWindowState = WindowState;
        }
    }
}
