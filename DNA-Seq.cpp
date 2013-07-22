//***************************************************************************************************************************************************************************************
//
//	NOMBRE DEL ARCHIVO: DNA-Seq.cpp
//	FECHA: 23-1-2012
//	AUTOR:	Miguel Catalan Bañuls
//	DESCRIPCIÓN: Secuención de ADN
//                 
//      This work is licensed under a Creative Commons Attribution-NonCommercial-NoDerivs 3.0 Unported License.
//      Attribution — You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work).
//      Noncommercial — You may not use this work for commercial purposes.
//      No Derivative Works — You may not alter, transform, or build upon this work.
//
//
//***************************************************************************************************************************************************************************************

#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <ctype.h> //Libreria donde esta la funcion tolower()


using namespace cv;

//Declaracion de varibles globales de las imagenes
Mat img_original;
Mat img_bin;
Mat img_trabajo;
Mat img_AGO1;
Mat img_AGO2;
Mat img_AGO3;
Mat img_AGO4;
Mat img_AGO5;
Mat img_AGO6;
Mat img_falso;
Mat splash;
Mat menu;
Mat result;

//Definimos un fichero
FILE* pfile;

//Definimos los arrays
char data1[300];
char data2[300];
char data3[300];
char data4[300];
char data5[300];
char data6[300];


//VARIABLES
int origenx;
int origeny;
int finalx;
int finaly;
Point primero;
Point segundo;
int arriba, abajo, alto;
int ago,acolumna;
int pos=0;
int sp=0;
int comienzo, final;

//FUNCIONES
void dibuja();
void recorta();
void recuadro(int evento,int x,int y,int flags, void* param);
void binarizar (int valor);
void magnitudes();
void secuencia(Mat &PARAMETRO);
void grupo(Mat &img_AGO);
void Almacenar(char let);
void Escribir ();
void Splash();
void Menu();
void Resultado();
void Comparar();


//Comenzamos con el programa
int main()
{       
        
        //Inicializamos la ventana a mostrar
	namedWindow("Secuenciación ADN");
	
	//Inicializamos el entorno gráfico
	Splash();
	sleep(2);
	Menu();
        waitKey(0);
        
	//Limpiamos la posible existencia de otro archivo previo
	pfile = fopen("Secuencia.txt", "w");
	fclose(pfile);
	
	//Cargamos la imagen
	img_original=imread("image.png",0); //para trabajar
	img_falso=imread("image.png",1); //para mostrar al usuario
	
	//Mostramos la imagen a color
	imshow("Secuenciación ADN",img_falso);
	
	//El usuario recorta la tabla
	cvSetMouseCallback("Secuenciación ADN",recuadro,NULL);
	waitKey(0); //Esperamos que confirme
	
	//Recortamos el area seleccionada por el usuario
	recorta();
	
	//Binarizamos la imagen para poderla tratar
	binarizar(20);
	
	//analizamos las medidas de la imagen
	magnitudes();

        //Recorcamos la seccion de la imagen para cada uno de los AGO
	img_AGO1=img_trabajo(Rect(0,0,(img_trabajo.cols/6),img_trabajo.rows));
	img_AGO2=img_trabajo(Rect((img_trabajo.cols/6),0,(img_trabajo.cols/6),img_trabajo.rows));
	img_AGO3=img_trabajo(Rect(((img_trabajo.cols/6)*2),0,(img_trabajo.cols/6),img_trabajo.rows));
	
	img_AGO4=img_trabajo(Rect((img_trabajo.cols-((img_trabajo.cols/6)*3)),0,(img_trabajo.cols/6),img_trabajo.rows));
	img_AGO5=img_trabajo(Rect((img_trabajo.cols-((img_trabajo.cols/6)*2)),0,(img_trabajo.cols/6),img_trabajo.rows));
	img_AGO6=img_trabajo(Rect((img_trabajo.cols-(img_trabajo.cols/6)),0,(img_trabajo.cols/6),img_trabajo.rows));
	
	//Procesamos cada uno de los AGO
	ago=1;
	grupo(img_AGO1);
	
	//Procesamos cada uno de los AGO
	ago=2;
	grupo(img_AGO2);
	
	//Procesamos cada uno de los AGO
	ago=3;
	grupo(img_AGO3);
	
	//Procesamos cada uno de los AGO
	ago=4;
	grupo(img_AGO4);
	
	//Procesamos cada uno de los AGO
	ago=5;
	grupo(img_AGO5);
	
	//Procesamos cada uno de los AGO
	ago=6;
	grupo(img_AGO6);
	
	//Comparamos los AGO para sacar mutaciones y el intro
	Comparar();
	
	//Escribimos los datos en el fichero
	Escribir();
	
	//Mostramos al usuario que el proceso se ha realizado
	Resultado();
	
	waitKey(0);
	return(0);
	
}

//Funcion llamada por SetMouseCallBack
void recuadro(int evento,int x,int y,int flags, void* param)
{
        //Accion cuando "aprieta" el boton izquierdo del raton
	if (evento==1)
	{
		origenx=x;
		origeny=y;
	}
	
	//Accion cuando "suelta" el boton izquierdo del raton
	else if (evento==4)
	{
		finalx=x;
		finaly=y;
		primero.x =  origenx;
		primero.y =  origeny;
		segundo.x =  finalx;
		segundo.y =  finaly;
		dibuja(); //llamamos directamente a la funcion dibujar
	}
	
}
//Esta funcion debe ir a en la libreria
void dibuja()
{       
	//Definimos un color
	CvScalar color = CV_RGB(250,1,1);
	
	//Dibujamos el rectangulo para comprobar que todo esta correcto
	rectangle(img_falso, primero, segundo, color,1,CV_AA,(int)0);
	imshow("Secuenciación ADN",img_falso);	
}

void recorta()
{
	//Recortamos la imagen que hemos recuadrado
	img_bin=img_original(Rect(origenx,origeny,abs(origenx-finalx),abs(origeny-finaly)));	
}

void binarizar (int valor)
{
        //Binarizamos con el umbral seleccionado
	for(int i=0;i<img_bin.rows;i++)
	{
		for(int j=0;j<img_bin.cols;j++)
		{
			if((img_bin.at<uchar>(i,j))>valor)
			{
				img_bin.at<uchar>(i,j)=0;
			}
			
			else
			{
				img_bin.at<uchar>(i,j)=255;
			}
		}
	}
}

void magnitudes()
{	

	int menor = 300; //valor grande para que no tome por defecto el 0 y no pueda afectar al ancho
	int mayor, ancho;
	for(int i=0;i<img_bin.rows;i++)
	{
		for(int j=0;j<img_bin.cols;j++)
		{
			if((img_bin.at<uchar>(i,j))<255)
			{
				
			}
			
			else if(img_bin.at<uchar>(i,j)==255 && menor>j)
			{
				menor=j;
			}
			
			else if(img_bin.at<uchar>(i,j)==255 && mayor<j)
			{
				mayor=j;
			}
		}
	}
	ancho=mayor-menor;
	//Nos quedamos solo con los nucleotidos
	img_trabajo=img_bin(Rect(menor,0,ancho,img_bin.rows));


}

void grupo(Mat &img_AGO)
{
        Mat img_grupo;
	int gmenor=300, gmayor,gancho;
	
	for(int i=0;i<img_AGO.rows;i++)
	{
		for(int j=0;j<img_AGO.cols;j++)
		{
			if((img_AGO.at<uchar>(i,j))<255)
			{
				
			}
			
			else if(img_AGO.at<uchar>(i,j)==255 && gmenor>j)
			{
				gmenor=j;
			}
			
			else if(img_AGO.at<uchar>(i,j)==255 && gmayor<j)
			{
				gmayor=j;
			}
		}
	}
	gancho=gmayor-gmenor;						
	img_grupo=img_AGO(Rect(gmenor,0,gancho,img_AGO.rows));
	
	//Pasar a secuencia =D
	secuencia(img_grupo);					
}

void secuencia(Mat &PARAMETRO)
{
	Mat img_grupo;
	PARAMETRO.copyTo(img_grupo);
	int prueba=0;
	char letra;
	
	//Dividimos el AGO en sus cuatro componentes
	int acolumna=img_grupo.cols/4;
	
	
	for(int i=img_grupo.rows;i>0;i--)
	{
		for(int j=0;j<img_grupo.cols;j++)
		{
		        //Si no detecta un nucleotido, no hacemos nada
			if((img_grupo.at<uchar>(i,j))<255)
			{
				
			}
			
			//Si detecta un nucleotido y se encuentra en la primera columna
			else if(img_grupo.at<uchar>(i,j)==255 && j>0 && j<(acolumna-3))
			{
				letra='A';
				prueba=1;
			}
			
			//Si detecta un nucleotido y se encuentra en la segunda columna
			else if(img_grupo.at<uchar>(i,j)==255 && j>((acolumna)+3) && j<((acolumna*2)-3))
			{
				letra='C';
				prueba=1;
			}
			
			//Si detecta un nucleotido y se encuentra en la tercera columna
			else if(img_grupo.at<uchar>(i,j)==255 && j>((acolumna*2)+2) && j<((acolumna*3)-2))
			{
				letra='G';
				prueba=1;
			}
			
			//Si detecta un nucleotido y se encuentra en la ultima columna
			else if(img_grupo.at<uchar>(i,j)==255 && j>((acolumna*3)+3) && j<((acolumna*4)))
			{
				letra='T';
				prueba=1;
			}
		}
		
		//Solo si hay nucleotido imprimimos
		if(prueba==1)
		{
		        Almacenar(letra);
		        pos++;		        
			prueba=0; //ponemos el semaforo en rojo
			i--;//Nos saltamos la siguiente linea para evitar el grosor de las lineas (¿Tolerancia? a fallos que hay que modificar)
			fflush(stdout);
		}
	}
	pos=0;
	
	
}

void Almacenar(char let)
{
        if(ago==1)
        {
                data1[pos]=let;
        }
        if(ago==2)
        {
                data2[pos]=let;
        }
        if(ago==3)
        {
                data3[pos]=let;
        }
        if(ago==4)
        {
                data4[pos]=let;
        }
        if(ago==5)
        {
                data5[pos]=let;
        }
        if(ago==6)
        {
                data6[pos]=let;
        }
        
}

void Comparar()
{
        //Varaibles varias
        int salir=0;
        int len1=strlen(data1);
        
        //Comparamos los tres tipode de AGO de cada ADN para ver las mutaciones, las cuales cambiaremos a minusculas.
        for(int i=0;i<len1;i++)
        {
                if(data1[i]!=data2[i])
                {
                        char let;
                        let=data2[i];
                        let=tolower(let); //pasar a minusculas
                        data2[i]=let; 
                }
                if(data1[i]!=data3[i])
                {
                        char let;
                        let=data3[i];
                        let=tolower(let); //pasar a minusculas
                        data3[i]=let; 
                }
                
        }
        int len2=strlen(data4);
        for(int i=0;i<len2;i++)
        {
                if(data4[i]!=data5[i])
                {
                        char let;
                        let=data5[i];
                        let=tolower(let); //pasar a minusculas
                        data5[i]=let; 
                }
                if(data4[i]!=data6[i])
                {
                        char let;
                        let=data6[i];
                        let=tolower(let); //pasar a minusculas
                        data6[i]=let; 
                }
                
        }        
                //Comparamos los AGO1 de cada tipo de ADN para saber donde comienza la modificacion ADNc
                for(comienzo=0;comienzo<strlen(data1);comienzo++)
                {
                        if(data1[comienzo]!=data4[comienzo])
                        {
                        break;
                        }
                }       
                comienzo++; //Sumamos uno al esta variable puesto que se ha inicializado en 0 
                
                //Puesto que en ADNc se suprime una parte de la informacion intentamos sacar esa parte que solo esta disponible en ADNg
                //Para ello vamos recorriendo el AGO1 de ADNg hasta que esta vuelva a seguir la secuencia donde se quedo el ADGO1 de ADNc (Comparamos unos pocos valores como muestra para decidir cuando termina)
                for(final=comienzo;final<strlen(data1);final++)
                {
                        if(data1[final]==data4[comienzo] && data1[final+1]==data4[comienzo+1] && data1[final+2]==data4[comienzo+2] && data1[final+3]==data4[comienzo+3] && data1[final+4]==data4[comienzo+4] && data1[final+5]==data4[comienzo+5])
                        {
                                break;
                        }
                }
                
}       

void Escribir ()
{
        //Abrimos el fichero y escribimos al final de el
	pfile = fopen("Secuencia.txt", "a+");
	
	//Poca explicacion hace falta, mas que nada es todo formato
	
	fprintf (pfile, "Secuencia de ADNg:\n");
	fprintf (pfile, "AGO1:\n");
	int len=strlen(data1);
	for(int i=0;i<len;i++)
	{
	        fprintf(pfile, "%c",data1[i]);
	}
	
	fprintf (pfile, "\n\nAGO1-27:\n");
	len=strlen(data2);
	for(int i=0;i<len;i++)
	{
	        fprintf(pfile, "%c",data2[i]);
	}
	
	fprintf (pfile, "\n\nAGO1-52:\n");
	len=strlen(data3);
	for(int i=0;i<len;i++)
	{
	        fprintf(pfile, "%c",data3[i]);
	}
	
	fprintf (pfile, "\n\n\nSecuencia de ADNc:\n");
	fprintf (pfile, "AGO1:\n");
	len=strlen(data4);
	for(int i=0;i<len;i++)
	{
	        fprintf(pfile, "%c",data4[i]);
	}
	
	fprintf (pfile, "\n\nAGO1-27:\n");
	len=strlen(data5);
	for(int i=0;i<len;i++)
	{
	        fprintf(pfile, "%c",data5[i]);
	}
	
	fprintf (pfile, "\n\nAGO1-52:\n");
	len=strlen(data6);
	for(int i=0;i<len;i++)
	{
	        fprintf(pfile, "%c",data6[i]);
	}
	fprintf (pfile, "\n\n\nThe intron starts at: %d",comienzo);
	fprintf (pfile, "\n\n\nThe intron ends at: %d",final);
	fclose(pfile);	
}

void Splash()
{
        //Mostramos Splash
        splash=imread("./Images/Splash.png",1);
        imshow("Secuenciación ADN",splash);
        waitKey(0); //La idea era que una vez cargado el Splash se esperase 2seg (con el sleep situado en el main) y despues saltase el menu, pero si no pongo el waitkey se salta el Splash
        //Creo que es debido a que entra en el sleep antes de que llegue a cargar la ventana con la imagen

}

void Menu()
{
        //Mostramos el menu
        menu=imread("./Images/Menu.png",1);
        imshow("Secuenciación ADN",menu);
}
void Resultado()
{
        //Mostramos ventana de resultado
        result=imread("./Images/Resultado.png",1);
        imshow("Secuenciación ADN",result);
}
