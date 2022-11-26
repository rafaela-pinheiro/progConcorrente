/*
 * Rafaela C. M. Pinheiro
 * DRE: 121038166
 */

import java.util.Random;
import java.util.Scanner;

class Buffer {
    private int tam;
    private Double buffer[];
    private int count, in, out;

    // construtor
    Buffer(int tam) {
        this.tam = tam;
        buffer = new Double[tam];
        count = 0;
        in = 0;
        out = 0;
    }

    public synchronized void Insere(Double elem) {
        try {
            while (count == tam) {
                // thread espera se o buffer está cheio
                System.out.println(elem + " não foi inserido porque não há espaço - Thread bloqueada");
                wait();
            }
            // buffer tem posição disponível, pode inserir
            buffer[in] = elem;
            System.out.println("Inserção de " + elem + " em buffer[" + in + "]");
            in = (in + 1) % tam;
            count++;
            notifyAll();
        } catch (InterruptedException e) {
            System.out.println("-- Erro : Insere()");
        }
    }

    public synchronized Double Remove() {
        try {
            while (count == 0) {
                // se buffer está vazio, não podemos remover. thread bloqueada
                System.out.println("Buffer vazio - Thread bloqueada");
                wait();
            }
            Double elem = buffer[out];
            System.out.println("Remoção de " + elem + " de buffer[" + out + "]");
            out = (out + 1) % tam;
            count--;
            notifyAll();
            return elem;
        } catch (InterruptedException e) {
            System.out.println("-- Erro : Remove()");
            return -1.;
        }
    }

    public void ImprimeBuffer() {
        for (int i = out; i < in; i = (i + 1) % tam) {
            System.out.println("buffer[" + i + "] = " + buffer[i]);
        }
    }
}

class Produtor extends Thread {
    private Buffer buffer;
    private Double elem;
    Random rand;
    private int id;

    // construtor
    Produtor(Buffer buffer, int id) {
        this.buffer = buffer;
        this.id = id;
    }

    public void run() {
        try {
            rand = new Random();
            for (;;) {
                elem = rand.nextDouble();
                System.out.println("\nThread " + id + " quer inserir o elemento " + elem);
                buffer.Insere(elem);
                sleep(1000);
            }
        } catch (InterruptedException e) {
            System.out.println("-- Erro : Produtor.run()");
            return;
        }
    }

}

class Consumidor extends Thread {
    private Buffer buffer;
    private Double elem;
    private int id;

    // construtor
    Consumidor(Buffer buffer, int id) {
        this.buffer = buffer;
        this.id = id;
    }

    public void run() {
        try {
            for (;;) {
                System.out.println("\nThread " + id + " quer remover um elemento");
                elem = buffer.Remove();
                sleep(1000);
            }
        } catch (InterruptedException e) {
            System.out.println("-- Erro : Consumidor.run()");
            return;
        }
    }
}

class Main {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        // lê tamanho do buffer do terminal
        System.out.println("Insira o tamanho do buffer: ");
        int tam = sc.nextInt();

        // cria buffer
        Buffer buffer = new Buffer(tam);

        // lê informações das threads do terminal
        System.out.println("Insira a quantidade de produtores: ");
        int nProds = sc.nextInt();
        System.out.println("Insira a quantidade de consumidores: ");
        int nCons = sc.nextInt();

        int nThreads = nProds + nCons;

        // reserva vetor de threads
        Thread[] threads = new Thread[nThreads];

        // cria threads produtoras
        for (int i = 0; i < nProds; i++) {
            threads[i] = new Produtor(buffer, i);
        }

        // cria threads consumidoras
        for (int i = nProds; i < nThreads; i++) {
            threads[i] = new Consumidor(buffer, i);
        }

        // inicia as threads
        for (int i = 0; i < nThreads; i++) {
            threads[i].start();
        }

        // espera as threads terminarem
        for (int i = 0; i < nThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }

        // termina scanner
        sc.close();
    }
}
