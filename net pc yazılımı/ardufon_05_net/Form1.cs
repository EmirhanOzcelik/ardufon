using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace ardufon_05_net
{
    public partial class Form1 : Form
    {

        int[] a = { 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200 };
        const int sn_gecikme = -1;
        string[] hafta_gun = { "Pazartesi", "Salı", "Çarşamba", "Perşembe", "Cuma", "Cumartesi", "Pazar" };
        string[] hafta_gun_ing = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

        public Form1()
        {
            InitializeComponent();
        }

        void yaz(String s) => textBox1.AppendText(s);

        private void comboBox1_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            String[] s = SerialPort.GetPortNames();
            foreach (String s2 in s)
            {
                comboBox1.Items.Add(s2);
            }
            comboBox2.Items.Clear();
            foreach(int a  in a)
            {
                comboBox2.Items.Add(a.ToString());
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            button1.Text = "bağlan";
            comboBox2.Text = "Baud Rate";
            comboBox1.Text = "Port";
            CheckForIllegalCrossThreadCalls = false;
            textBox1.ReadOnly = true;
            textBox1.ScrollBars = ScrollBars.Both;
            textBox2.ReadOnly = true;
            textBox2.ScrollBars = ScrollBars.Both;
            textBox1.TabStop = false; textBox2.TabStop = false;
            comboBox1.TabStop = false; comboBox2.TabStop = false;
            try
            {
                if (serialPort1.IsOpen)
                {
                    serialPort1.Close();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                if (!serialPort1.IsOpen)
                {
                    if (comboBox2.SelectedItem != null)
                    {
                        serialPort1.BaudRate = a[comboBox2.SelectedIndex];
                    }
                    else
                    {
                        serialPort1.BaudRate = 9600;
                    }
                    if (comboBox1.SelectedItem == null)
                    {
                        serialPort1.PortName = "COM4";
                    }
                    else
                    {
                        serialPort1.PortName = comboBox1.SelectedItem.ToString();
                    }
                    serialPort1.Open();
                    yaz("Adres = " + serialPort1.PortName + Environment.NewLine);
                    yaz("Baud Rate = " + serialPort1.BaudRate);
                    yaz("   ***   ");
                    button1.Text = "kes";
                    button1.BackColor = Color.Green;
                }
                else
                {
                    serialPort1.Close();
                    button1.Text = "bağlan";
                    button1.BackColor = Color.White;
                    yaz("bağlantı istemlice kesildi");
                    yaz("   ***   ");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            textBox2.AppendText(serialPort1.ReadLine() + Environment.NewLine);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            saat_tarih_yaz();
        }
        void saat_tarih_yaz()
        {
            int sn = DateTime.Now.Second;
            int dk = DateTime.Now.Minute;
            int saat = DateTime.Now.Hour;
            string hafatgunu = DateTime.Now.DayOfWeek.ToString();  
            int ay_gun = DateTime.Now.Day;
            int ay = DateTime.Now.Month;
            int yil = DateTime.Now.Year;
            label5.Text = sn.ToString();
            label3.Text = dk.ToString();
            label2.Text = saat.ToString();
            label8.Text = ay_gun.ToString();
            label10.Text = ay.ToString();
            label11.Text = yil.ToString();
            for (int a = 0; a < hafta_gun_ing.Length; a++) 
            {
                if(DateTime.Now.DayOfWeek.ToString() == hafta_gun_ing[a])
                {
                    label13.Text = hafta_gun[a];
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
            string sn = "T" + (DateTime.Now.Second - sn_gecikme).ToString();
            string dk = "T" + (100 + DateTime.Now.Minute).ToString();
            string saat = "T" + (200 + DateTime.Now.Hour).ToString();
            string ay_gun = "T" + (300 + DateTime.Now.Day).ToString();
            string ay = "T" + (400 + DateTime.Now.Month).ToString();
            string yil = "Y" + DateTime.Now.Year.ToString();
            string gun = 0.ToString();
            for (int a = 0; a < hafta_gun_ing.Length; a++)
            {
                if (DateTime.Now.DayOfWeek.ToString() == hafta_gun_ing[a])
                {
                    gun = "Y" + a.ToString();
                }
            }
                serialPort1.Write(sn); //t sn
                serialPort1.Write(",");
                serialPort1.Write(dk); //t 100 + dk
                serialPort1.Write(",");
                serialPort1.Write(saat); //t 200 + saat
                serialPort1.Write(",");
                serialPort1.Write(gun); //y gun
                serialPort1.Write(",");
                serialPort1.Write(ay_gun); //t 300 + ay_gun
                serialPort1.Write(",");
                serialPort1.Write(ay); //t 400 + ay
                serialPort1.Write(",");
                serialPort1.Write(yil); //y yil
                serialPort1.Write("*");
            }
            catch(Exception ex) 
            {
                MessageBox.Show(ex.ToString());
            }
        }


    }
}
