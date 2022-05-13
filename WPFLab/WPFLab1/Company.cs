using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Data;

namespace WPFLab1
{
    public class Branch
    {
        public string BranchName { get; set; }
        public string CompanyName { get; set; }
        public string Address { get; set; }
        public string Email { get; set; }
        public string PhoneNo { get; set; }
        public string WebSite { get; set; }
    }

    public class Company
    {
        public string CompanyName { get; set; }
        public string Phrase { get; set; }
        public string Description { get; set; }
        public List<Branch> Branches { get; set; }
    }
}
