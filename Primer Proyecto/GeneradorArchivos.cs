using System;
using System.IO;

namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            string userPath = "C:\\Users\\ACER\\source\\repos\\ConsoleApp1\\";
            string archive = "File.csv";
            string absolutePath = @userPath + "/" + archive;
            int rowNumber = 2;
            int columnNumber = 200;
            int asciiValue;
            int rand;
            Random rnd = new Random();
            Random rnd2 = new Random();

            Console.WriteLine("Enter the row number \n");
            rowNumber = Int32.Parse(Console.ReadLine());

            Console.WriteLine("Enter Column number \n");
            columnNumber = Int32.Parse(Console.ReadLine());

            if (File.Exists(absolutePath))
            {
                File.Delete(absolutePath);
            }

            var file = File.Create(absolutePath);
            file.Close();

            using (StreamWriter sw = new StreamWriter(@absolutePath, true))
            {
                for (int i = 0; i <= rowNumber; i++)
                {
                    for (int j = 0; j <= columnNumber - 1; j++)
                    {
                        if ((j == 0) && (i != 0))
                        {
                            
                            rand = rnd2.Next(1,3);
                            if (rand == 1)
                            { asciiValue = rnd.Next(65, 91);
                                sw.Write((char)asciiValue);
                            }
                            if (rand == 2)
                            { asciiValue = rnd.Next(97, 123);
                                sw.Write((char)asciiValue);
                            }
                            
                        
                        }
                        else if (i == 0)
                        {
                            if (j == 0)
                            {
                                sw.Write("C" + (j + 1));
                            }
                            else
                            {
                                sw.Write(",C" + (j + 1));
                            }
                        }
                        else
                        {
                            
                            int rand1 = 0;
                            rand1 = rnd2.Next(1, 3);
                            if (rand1 == 1)
                            {
                                asciiValue = rnd.Next(65, 91);
                                sw.Write(","+(char)asciiValue);
                            }
                            if (rand1 == 2)
                            {
                                asciiValue = rnd.Next(97, 123);
                                sw.Write(","+(char)asciiValue);
                            }
                            
                        }
                    }
                    sw.Write("\n");
                }
            }
        }
    }
}
