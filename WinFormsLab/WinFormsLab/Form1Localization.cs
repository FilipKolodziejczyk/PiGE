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
        // Changes language to english on the fly
        private void englishButton_Click(object sender, EventArgs e)
        {
            if (englishButton.Checked == false)
            {
                polishButton.Checked = false;
                englishButton.Checked = true;
                CultureInfo.CurrentUICulture = new CultureInfo("en", false);
                Reload();
            }
        }

        // Changes language to polish on the fly
        private void polishButton_Click(object sender, EventArgs e)
        {
            if (polishButton.Checked == false)
            {
                englishButton.Checked = false;
                polishButton.Checked = true;
                CultureInfo.CurrentUICulture = new CultureInfo("pl-PL", false);
                Reload();
            }
        }

        // Only toolstrip items and groupbox must be reloaded (only objects affected by localization)
        // In case of more complex app, we should iterate through all controls recursively
        private void Reload()
        {
            SuspendLayout();
            
            ComponentResourceManager resources = new ComponentResourceManager(typeof(Form1));
            resources.ApplyResources(groupBox1, groupBox1.Name);
            groupBox1.Invalidate();
            foreach (ToolStripItem item in toolStrip1.Items)
            {
                if (item == thicknessComboBox)
                {
                    int index = thicknessComboBox.SelectedIndex;
                    thicknessComboBox.Items.Clear();
                    thicknessComboBox.Items.AddRange(new object[] {
                        resources.GetString("thicknessComboBox.Items"),
                        resources.GetString("thicknessComboBox.Items1"),
                        resources.GetString("thicknessComboBox.Items2")});
                    thicknessComboBox.SelectedIndex = index;
                }
                resources.ApplyResources(item, item.Name);
                item.Invalidate();
            }
            
            ResumeLayout();
        }
    }
}
