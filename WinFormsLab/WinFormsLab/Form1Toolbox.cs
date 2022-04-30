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
    enum Thickness : int
    {
        Small = 0,
        Medium = 1,
        Big = 2
    };

    public partial class Form1 : Form
    {
        private Button choosenColorButton;
        private ToolStripButton? selectedTool;

        // Line size configuration
        private int lineThickness
        {
            get
            {
                switch ((Thickness)thicknessComboBox.SelectedIndex)
                {
                    case Thickness.Small:
                        return 1;
                    case Thickness.Medium:
                        return 2;
                    case Thickness.Big:
                        return 3;
                    default:
                        return 2;
                }
            }
        }

        // Changing drawing color
        private void colorButton_Click(object sender, EventArgs e)
        {
            SuspendLayout();
            
            Button button = (Button)sender;
            Button oldButton = choosenColorButton;
            choosenColorButton = button;
            choosenColorBox.BackColor = button.BackColor;
            
            // Changing borders
            oldButton.Invalidate();
            button.Invalidate();
            
            ResumeLayout();
        }

        // Selecting drawing tool
        private void toolButton_Click(object sender, EventArgs e)
        {
            SuspendLayout();

            ToolStripButton newTool = (ToolStripButton)sender;
            if (selectedTool == newTool)
            {
                selectedTool.Checked = false;
                selectedTool = null;
            }
            else
            {
                if (selectedTool != null)
                    selectedTool.Checked = false;
                selectedTool = newTool;
                selectedTool.Checked = true;
            }

            ResumeLayout();
        }

        // Saving current state of canva
        private void saveButton_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog saveDialog = new SaveFileDialog())
            {
                saveDialog.Filter = "Bitmap Image(*.bmp)|*.bmp|JPEG Image(*.jpg)|*.jpg|PNG Image(*.png)|*.png";
                if (saveDialog.ShowDialog() == DialogResult.OK)
                    pictureBox1.Image.Save(saveDialog.FileName);
            }
        }

        // Opening image form file and resizing form to fit it
        private void loadButton_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openDialog = new OpenFileDialog())
            {
                openDialog.Filter = "Bitmap Image(*.bmp)|*.bmp|JPEG Image(*.jpg)|*.jpg|PNG Image(*.png)|*.png";
                if (openDialog.ShowDialog() == DialogResult.OK)
                {
                    Bitmap image = new Bitmap(openDialog.FileName);
                    
                    ActiveForm.Size = new Size(image.Width * (int)pictureToFormRatio.Width, image.Height * (int)pictureToFormRatio.Height);
                    pictureBox1.Image.Dispose();
                    pictureBox1.Image = image;
                    // Without turning off maximize
                    if (WindowState == FormWindowState.Maximized)
                        Form1_ResizeEnd(this, null);
                }
            }
        }

        // Clearing canva with white color
        private void clearButton_Click(object sender, EventArgs e)
        {
            Graphics canvaGraphics = Graphics.FromImage(pictureBox1.Image);
            canvaGraphics.Clear(Color.White);
            pictureBox1.Refresh();
        }
        
    }
}
