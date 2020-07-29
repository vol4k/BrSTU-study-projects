using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace LAB08
{
    class Program
    {
        static void Main()
        {
            int number = 10;
            int genes = 8;
            int count = 9;
            Beings beings = new Beings(number, genes, count);
        }
    }

    class Beings
    {
        private Mutex print;
        private Mutex migration;
        private Mutex[] mutex;
        private volatile int queue = 0;
        private Random Rand = new Random();
        readonly int quantity;
        readonly int genes;
        readonly int total;
        private int[] gener;

        private List<List<bool>>[] beings;

        public Beings(int q, int g, int t)
        {
            gener = new int[t];
            quantity = q;
            genes = g;
            total = t;

            mutex = new Mutex[t];
            print = new Mutex();
            migration = new Mutex();

            beings = new List<List<bool>>[t];

            for (int p = 0; p < t; p++)  {
                mutex[p] = new Mutex();
                gener[p] = 0;

                for (int i = 0; i < t; i++)
                {
                    beings[p] = new List<List<bool>>();
                }

                for (int i = 0; i < quantity; i++)
                {
                    List<bool> being = new List<bool>();
                    for (int j = 0; j < genes; j++)
                    {
                        being.Add(Rand.Next() % 2 == 1);
                    }
                    beings[p].Add(being);
                }
                StartAsync();
            }
            UpdateRandAsync();
            Console.ReadKey();
        }
        private List<int> Assessment(int p)
        {
            List<int> marks = new List<int>();
            for (int i = 0; i < beings[p].Count; i++)
            {
                int mark = 0;
                for (int j = 0; j < beings[p][i].Count; j++)
                {
                    if (beings[p][i][j] == true)
                    {
                        mark++;
                    }
                }
                marks.Add(mark);
            }
            return marks;
        }
        private List<int> Assessment(List<List<bool>> tempGeneration)
        {
            List<int> marks = new List<int>();
            for (int i = 0; i < tempGeneration.Count; i++)
            {
                int mark = 0;
                for (int j = 0; j < tempGeneration[i].Count; j++)
                {
                    if (tempGeneration[i][j] == true)
                    {
                        mark++;
                    }
                }
                marks.Add(mark);
            }
            return marks;
        }
        private List<int> DistanceCalculation(int p, int Choice)
        {
            List<int> distances = new List<int>();
            for (int i = 0; i < beings[p].Count; i++)
            {
                if (i != Choice)
                {
                    int dist = 0;
                    for (int j = 0; j < beings[p][i].Count; j++)
                    {
                        if (beings[p][Choice][j] != beings[p][i][j])
                        {
                            dist++;
                        }
                    }
                    distances.Add(dist);
                }
            }
            return distances;
        }
        private void Reproduction(int p)
        {
            mutex[p].WaitOne();
            gener[p]++;
            int choice = Rand.Next() % beings[p].Count;
            List<List<bool>> tempGeneration = new List<List<bool>>();
            List<int> dists = DistanceCalculation(p, choice);
            int indexOfMax = dists.IndexOf(dists.Max());
            int index1, index2;
            index1 = Rand.Next() % beings[p][choice].Count;
            do
            {
                index2 = Rand.Next() % beings[p][indexOfMax].Count;
            }
            while (index1 == index2);
            tempGeneration.Add(beings[p][choice]);
            tempGeneration.Add(beings[p][indexOfMax]);

            int dist = index1 - index2;
            int volume = beings[p][choice].Count;

            if (dist < 0) dist *= -1;

            for (int j = 0; j < dist; j++)
            {
                int index = (j + index1) % volume;
                bool temp = beings[p][choice][index];
                beings[p][choice][index] = beings[p][indexOfMax][index];
                beings[p][indexOfMax][index] = temp;
            }
            tempGeneration.Add(beings[p][choice]);
            tempGeneration.Add(beings[p][indexOfMax]);
            List<int> marks = Assessment(tempGeneration);
            List<bool> theBest = tempGeneration[marks.IndexOf(marks.Max())];
            beings[p].Clear();
            beings[p].Add(theBest);
            mutex[p].ReleaseMutex();
        }
        private void Mutation(int p)
        {
            mutex[p].WaitOne();
            int being = Rand.Next() % beings[p].Count;
            int first = Rand.Next() % beings[p][being].Count;
            int second = Rand.Next() % beings[p][being].Count;

            bool temp = beings[p][being][first];
            beings[p][being][first] = beings[p][being][second];
            beings[p][being][second] = temp;
            mutex[p].ReleaseMutex();
        }
        private void GenerateNewGeneration(int p)
        {
            mutex[p].WaitOne();
            while (beings[p].Count != quantity)
            {
                List<bool> being = new List<bool>();
                for (int i = 0; i < genes; i++)
                {
                    being.Add(Rand.Next() % 2 == 1);
                }
                beings[p].Add(being);
            }
            mutex[p].ReleaseMutex();
        }
        private void Migration(int p)
        {
            int i, j = 0;
            int cap = Convert.ToInt32(Math.Sqrt(total));
            List<List<int>> index = new List<List<int>>();
            for(i = 0; i < cap; i++)
            {
                index.Add(new List<int>());
                for(j = 0; j < cap; j++)
                {
                    index[i].Add(0);
                }
            }

            for (i = 0; i < cap; i++)
                for (j = 0; j < cap; j++)
                {
                    index[i][j] = i * cap + j;
                }

            int donor = 0;
            for (i = 0; i < cap; i++)
            {
                if((j = index[i].IndexOf(p)) != -1)
                {
                    break;
                }
            }
            switch (Rand.Next() % 4)
            {
                case 0: // right
                    ++j;
                    j%= cap;
                    donor = index[i][j];
                    break;
                case 1: // left
                    if (--j < 0)
                        j = cap - 1; 
                    donor = index[i][j];
                    break;
                case 2: // up
                    ++i;
                    i %= cap;
                    donor = index[i][j];
                    break;
                case 3: // down
                    if (--i < 0)
                        i = cap - 1;
                    donor = index[i][j];
                    break;
            }
            migration.WaitOne();
            mutex[p].WaitOne();
            mutex[donor].WaitOne();
            List<int> Fmarks = Assessment(p);
            List<int> Smarks = Assessment(beings[donor]);

            int SwapCount = Convert.ToInt32(total * 0.04);
            if(SwapCount == 0)
            {
                SwapCount = 1;
            }

            for(i = 0; i < SwapCount; i++)
            {
                int iF = Fmarks.IndexOf(Fmarks.Min());
                int iS = Smarks.IndexOf(Smarks.Min());

                Fmarks.Remove(Fmarks.Min());
                Smarks.Remove(Smarks.Min());

                List<bool> temp = beings[p][iF];
                beings[p][iF] = beings[donor][iS];
                beings[donor][iS] = temp;
            }
            mutex[donor].ReleaseMutex();
            mutex[p].ReleaseMutex();
            migration.ReleaseMutex();
        }
        private void Start(int p)
        {
            Init(p);

            List<int> marks;
            bool ok = true;
            do
            {
                Reproduction(p);
                Mutation(p);
                marks = Assessment(p);
                if (marks.Contains(genes) == true)
                {
                    print.WaitOne();
                    Console.WriteLine(
                        "Лучшая особь была создана\n" +
                        "Популяция: {0}\n" +
                        "Поколение: {1}.\n", p, gener[p]);
                    ok = !ok;
                    Thread.Sleep(1000);
                    print.ReleaseMutex();
                }
                GenerateNewGeneration(p);
                if (gener[p] % 10 == 0)
                {
                    Migration(p);
                }
            } while (ok);
        }

        private void Init(int p)
        {
            int last = 0;
            while (queue < beings.Length)
            {
                if (p == 0 && last != queue)
                {
                    Console.Clear();
                    Console.WriteLine("Инициализация... ({0}/{1})", queue, total);
                    last = queue;
                }
            }
            if (p == 0)
            {
               Console.Clear();
            }
            Thread.Sleep(500);
        }

        private async void StartAsync()
        {
            await Task.Run(() => Start(queue++));
            queue--;
        }
        private async void UpdateRandAsync()
        {
            await Task.Run(() => UpdateRand());
        }
        private void UpdateRand()
        {
            while (queue != 0)
            {
                if (Rand.Next() == Rand.Next())
                    Rand = new Random();
            }
        }
    }
}