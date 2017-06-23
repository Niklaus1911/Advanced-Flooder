using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace Spammer

{

    public partial class Form1 : Form
    {

        [DllImport("User32.dll")]
        static extern int SetForegroundWindow(IntPtr point);

        int delay;
        int
        var;
        int valido = 0;
        int max;
        string menu;
        int lbx;
        string rc;
        private delegate void del();
        string lbl;

        int check = 0;
        public Form1()

        {
            InitializeComponent();

        }

        private void button1_Click(object sender, EventArgs e)
        {

            label2.Text = "0";
            timer1.Start();
            this.Text = "Advanced Flooder - Active";
            label6.Text = "Active";
            label6.ForeColor = Color.Green;

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            var += 1;
            label2.Text =
              var.ToString();
            if (textBox2.Text ==
              var.ToString())
            {
                timer1.Stop();
                var = 0;

                label6.Text = "Inactive";
                label6.ForeColor = Color.Red;
                this.Text = "Advanced Flooder - Inactive";

            }

            Process p = Process.GetProcessesByName("notepad").FirstOrDefault();
            if (p != null)
            {
                IntPtr h = p.MainWindowHandle; //- sets the window 
                SetForegroundWindow(h); //- sets the window or anything 
                SendKeys.Send(richTextBox1.Text); //- this is message            
                Thread.Sleep(300); //- miliseconds before it hit ENTER
                SendKeys.SendWait("{ENTER}"); //- hit enter to a new line
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {

            timer1.Stop();
            var = 0;
            this.Text = "Advanced Flooder - Inactive";
            label6.Text = "Inactive";
            label6.ForeColor = Color.Red;
        }

        private void Form1_Load(object sender, EventArgs e)
        {

            Process[] processlist = Process.GetProcesses();

            foreach (Process theprocess in processlist)
            {

                ListViewItem item = new ListViewItem(theprocess.ProcessName);
                comboBox1.Items.Add(item.Text);

            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Process[] processlist = Process.GetProcesses();

            foreach (Process theprocess in processlist)
            {

                ListViewItem item = new ListViewItem(theprocess.ProcessName);
                comboBox1.ResetText();
                comboBox1.Items.Add(item.Text);

            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Advanced Flooder\r\nAuthor: Niklaus1911\r\nAll rights reserved.\r\nWebsite: www.igdownloader.com", "Advanced Flooder", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F5)
            {
                timer1.Start();
            }

            if (e.KeyCode == Keys.F6)
            {
                timer1.Stop();
            }
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void textBox2_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar))

            {
                e.Handled = true;
            }
        }

        private void fuori()
        {
            max = Int32.Parse(textBox2.Text) + 1;
            label2.Text = lbl;
            rc = richTextBox1.Text;

            menu = comboBox1.Text;

            lbx = Int32.Parse(textBox1.Text);
        }

        private void labelsix()
        {
            label6.Text = "Inactive";
            label6.ForeColor = Color.Red;
            this.Text = "Advanced Flooder - Inactive";
        }

        private void butto()
        {
            button4.Enabled = true;
            button6.Enabled = false;
        }

        private void Delegato()
        {
            del dl = fuori;
            del lb6 = labelsix;
            del but = butto;

            while (1 == 1)
            {
                if (check == 1)
                {
                    this.Invoke(dl);

                    Process p = Process.GetProcessesByName(menu).FirstOrDefault();
                    if (p != null)
                    {
                        IntPtr h = p.MainWindowHandle; //- sets the window 
                        SetForegroundWindow(h); //- sets the window or anything 
                        SendKeys.SendWait(rc); //- this is message            
                        Thread.Sleep(lbx); //- miliseconds before it hit ENTER

                        SendKeys.SendWait("{ENTER}"); //- hit enter to a new line

                    }

                    var += 1;
                    lbl =
                      var.ToString();
                    if (max ==
                      var)
                    {

                        var = 0;

                        this.Invoke(lb6);
                        this.Invoke(but);

                        break;
                    }
                    if (valido == 1)
                    {
                        valido = 0;
                        break;

                    }

                }
                else if (check == 0)
                {
                    this.Invoke(dl);

                    SendKeys.SendWait(rc); //- this is message            
                    Thread.Sleep(lbx); //- miliseconds before it hit ENTER

                    SendKeys.SendWait("{ENTER}"); //- hit enter to a new line

                    var += 1;
                    lbl =
                      var.ToString();
                    if (max ==
                      var)
                    {

                        var = 0;

                        this.Invoke(lb6);
                        this.Invoke(but);

                        break;
                    }
                    if (valido == 1)
                    {
                        valido = 0;
                        break;

                    }
                }

            }

        }

        private void button4_Click(object sender, EventArgs e)
        {
            int delay;
            delay = Int32.Parse(textBox1.Text);
            if (delay < 50)

            {
                MessageBox.Show("Minimum delay is 50 ms", "Advanced Flooder", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;

            }

            button4.Enabled = false;
            button6.Enabled = true;

            label2.Text = "0";
            var = 0;
            this.Text = "Advanced Flooder - Active";
            label6.Text = "Active";
            label6.ForeColor = Color.Green;
            Thread thread1 = new Thread(Delegato);
            thread1.Start();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            button4.Enabled = true;
            button6.Enabled = false;
            valido = 1;
            var = 0;
            this.Text = "Advanced Flooder - Inactive";
            label6.Text = "Inactive";
            label6.ForeColor = Color.Red;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            valido = 1;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar))

            {
                e.Handled = true;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
            {
                check = 1;
                button3.Enabled = true;
                comboBox1.Enabled = true;
                label1.Enabled = true;
            }
            else if (checkBox1.Checked == false)
            {
                check = 0;
                button3.Enabled = false;
                comboBox1.Enabled = false;
                label1.Enabled = false;
            }
        }
    }

}