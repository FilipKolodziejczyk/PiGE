using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml.Serialization;
using System.Xml;
using System.IO;
using System.Text.RegularExpressions;
using System.Net.Mail;
using System.Globalization;

namespace WPFLab1
{
    public partial class MainWindow : Window
    {
        static readonly string filepath = "Companies.xml";

        public MainWindow()
        {
            InitializeComponent();
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            FileStream fileStream = new FileStream(filepath, FileMode.Open);
            XmlSerializer xmlSerializer = new XmlSerializer(typeof(Company[]));
            XmlReader xmlReader = XmlReader.Create(fileStream);

            Company[] companies = (Company[])xmlSerializer.Deserialize(xmlReader);
            this.DataContext = companies;

            fileStream.Close();
        }
    }

    public class EmailValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
        {
            try
            {
                MailAddress email = new MailAddress((string)value);

                return new ValidationResult(true, null);
            }
            catch
            {
                return new ValidationResult(false, null);
            }
        }
    }

    public class PhoneNumberValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
        {
            if (((string)value).Any(c => c < '0' || c > '9'))
                return new ValidationResult(false, null);

            return new ValidationResult(true, null);
        }
    }

    public class DescriptionValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            if (((string)value).Length > 500)
                return new ValidationResult(false, null);

            return new ValidationResult(true, null);
        }
    }
}