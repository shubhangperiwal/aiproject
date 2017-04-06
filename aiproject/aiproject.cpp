#include<gtk/gtk.h>
#include<string.h>
#include<stdio.h>
#include<cairo.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<string.h>
#include<math.h>
#include<iostream>
#include<string.h>
#include<stdlib.h>
using namespace std;


// run using cc UI.c `pkg-config --cflags --libs gtk+-2.0`

char source_received[100],destination_received[100];
char str[100][100];
int top=0;

char tempo1[100][25];
char tempo2[100][25];
int temp1_k = 0;
int temp2_k = 0;
char final_display_result_gtk[10][40];
int final_display_counter = 0;

char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

int source_count = 0;
int destination_count = 0;
char mid_point[40];
typedef struct station
{
    int number_of_neighbors;
    struct station *neighbors[4];
    char station_name[25];
    char source_path[200], destination_path[200];
}STATION;

STATION visited_source[20];
STATION visited_destination[20];

struct station london_underground[10];

int check_temp1(char *c)
{
    for (int i = 0; i < temp1_k; i++)
    {
        if (strcmp(c, tempo1[i]) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int check_temp2(char *c)
{
    for (int i = 0; i < temp2_k; i++)
    {
        if (strcmp(c, tempo2[i]) == 0)
        {
            return 0;
        }
    }
    return 1;
}


void create_graph()
{
    char* station_names_list[10] = { "Oxford Circus","Bond Street","Marble Arch","Lancester Gate","Tottenham Court Road",
        "Warren Street","Green Park",    "Euston", "St. James Park", "Holborn" };
    int i;
   
    for (i = 0; i < 10; i++)
    {
        strcpy(london_underground[i].station_name, station_names_list[i]);
    }

    london_underground[0].neighbors[0] = &london_underground[1];
    london_underground[0].neighbors[1] = &london_underground[5];
    london_underground[0].neighbors[2] = &london_underground[6];
    london_underground[0].neighbors[3] = &london_underground[4];

    london_underground[0].number_of_neighbors = 4;
   
    london_underground[1].neighbors[0] = &london_underground[0];
    london_underground[1].neighbors[1] = &london_underground[2];

    london_underground[1].number_of_neighbors = 2;
   
    london_underground[2].neighbors[0] = &london_underground[1];
    london_underground[2].neighbors[1] = &london_underground[3];
    london_underground[2].number_of_neighbors = 2;

    london_underground[3].neighbors[0] = &london_underground[2];
    london_underground[3].neighbors[1] = &london_underground[8];
    london_underground[3].number_of_neighbors = 2;

    london_underground[4].neighbors[0] = &london_underground[0];
    london_underground[4].neighbors[1] = &london_underground[9];
    london_underground[4].number_of_neighbors = 2;

    london_underground[9].neighbors[0] = &london_underground[4];
    london_underground[9].number_of_neighbors = 1;

    london_underground[5].neighbors[0] = &london_underground[0];
    london_underground[5].neighbors[1] = &london_underground[7];
    london_underground[5].number_of_neighbors = 2;

    london_underground[6].neighbors[0] = &london_underground[0];
    london_underground[6].neighbors[1] = &london_underground[8];
    london_underground[6].number_of_neighbors = 2;

    london_underground[7].neighbors[0] = &london_underground[5];
    london_underground[7].number_of_neighbors = 1;

    london_underground[8].neighbors[0] = &london_underground[6];
    london_underground[8].neighbors[1] = &london_underground[3];
   
    london_underground[8].number_of_neighbors = 2;

}

int find_address(char *c)
{
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(c, london_underground[i].station_name) == 0)
            return i;

    }
    cout << "Not found so exiting code ";
    exit(0);
}


void bfs_source(STATION node)
{
    int i = 0;
    int flagchecker = 1;
    for (i = 0; i < node.number_of_neighbors; i++)
    {
        /*
        for (int j = 0; j < source_count; j++)
        {
            if (strcmp(node.neighbors[i]->station_name, visited_source[j].station_name) == 0)
                flagchecker = 0;
        }
        */
        if (flagchecker)
        {
            STATION *temp = node.neighbors[i];
            visited_source[source_count] = *temp;
           
            strcat(visited_source[source_count].source_path, node.source_path);
            strcat(visited_source[source_count].source_path, node.station_name);
            strcat(visited_source[source_count].source_path, "        ");

            source_count++;

        }
    }
}

void bfs_destination(STATION node)
{
    int i = 0;
    int flagchecker = 1;
    for (i = 0; i < node.number_of_neighbors; i++)
    {
        /*for (int j = 0; j < destination_count; j++)
        {
            if (strcmp(node.neighbors[i]->station_name, visited_destination[j].station_name) == 0)
                flagchecker = 0;
        }
        */
        if (flagchecker)
        {
            STATION *temp = node.neighbors[i];
            visited_destination[destination_count] = *temp;
           
            strcat(visited_destination[destination_count].destination_path, node.destination_path);
            strcat(visited_destination[destination_count].destination_path, strrev(node.station_name));
           

            strcat(visited_destination[destination_count].destination_path, "      ");

            destination_count++;
        }
    }
}

void display_path_source(STATION node,char *midway)
{
    cout << node.source_path;
    int j = 0;
    int count = 0;
    int k = 0;
    strcpy(mid_point,midway);
    for (int i = 0; i < strlen(node.source_path); i++)
    {
       
        if (node.source_path[i] == ' '&&node.source_path[i + 1] == ' '&& node.source_path[i + 2] == ' ')
        {
            while ((node.source_path[i]==' '))
                i++;
            j++;
            k = 0;
        }
       
            final_display_result_gtk[j][k] = node.source_path[i];
            k++;
       
    }

    cout << " " << midway<<" ";
    strcpy(final_display_result_gtk[j], midway);
    j++;
    final_display_counter = j;
    //cout << node.destination_path<<" ";
}
void display_path_destination(STATION node)
{
    int k = 0;
    cout << strrev(node.destination_path);
    //cout << node.destination_path;
    int w=0;
    while (node.destination_path[w] == ' ')
        w++;

    for (int i = w; i < strlen(node.destination_path); i++)
    {
        if (node.destination_path[i] == ' '&&node.destination_path[i + 1] == ' '&& node.destination_path[i + 2] == ' ')
        {
            while (node.destination_path[i] == ' ')
                i++;
               
                final_display_counter++;
                k = 0;
        }
        final_display_result_gtk[final_display_counter][k] = node.destination_path[i];
        k++;
    }
   
}

void final_gtk_display()
{
    cout << "2d array display " << endl << endl;
    for (int i = 0; i <= final_display_counter; i++)
    {
        if(final_display_result_gtk[i][0]<=122 && final_display_result_gtk[i][0]>=97)
            strrev(final_display_result_gtk[i]);
        cout << final_display_result_gtk[i]<<endl;
    }
    cout << final_display_counter;
}

void display_graphs()
{
    for (int i = 0; i < 10; i++)
    {
        cout << endl;
        cout << "Station " << i<<endl;
        cout << london_underground[i].station_name<<" "<<london_underground[i].number_of_neighbors<<endl;
        for (int j = 0; j < london_underground[i].number_of_neighbors; j++)
        {
            cout << london_underground[i].neighbors[j]->station_name << endl;
        }
    }
}



static void button_clicked(GtkWidget** entry, GtkWidget* widget)
{
}

static void source1(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Oxford Circus");
    strcpy(source_received,"Oxford Circus");
}
static void source2(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Bond Street");
    strcpy(source_received,"Bond Street");
}
static void source3(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Marble Arch");
    strcpy(source_received,"Marble Arch");
}
static void source4(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Lancester Gate");
    strcpy(source_received,"Lancester Gate");
}
static void source5(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Tottenham Court Road");
    strcpy(source_received,"Tottenham Court Road");
}
static void source6(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Warren Street");
    strcpy(source_received,"Warren Street");
}
static void source7(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Green Park");
    strcpy(source_received,"Green Park");
}
static void source8(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Euston");
    strcpy(source_received,"Euston");
}
static void source9(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  St. James Park");
    strcpy(source_received,"St. James Park");
}
static void source10(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Holborn");
    strcpy(source_received,"Holborn");
}

static void source11(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Oxford Circus");
    strcpy(destination_received,"Oxford Circus");
}
static void source21(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Bond Street");
    strcpy(destination_received,"Bond Street");
}
static void source31(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Marble Arch");
    strcpy(destination_received,"Marble Arch");
}
static void source41(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Lancester Gate");
    strcpy(destination_received,"Lancester Gate");
}
static void source51(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Tottenham Court Road");
    strcpy(destination_received,"Tottenham Court Road");
}
static void source61(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Warren Street");
    strcpy(destination_received,"Warren Street");
}
static void source71(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Green Park");
    strcpy(destination_received,"Green Park");
}
static void source81(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Euston");
    strcpy(destination_received,"Euston");
}
static void source91(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  St. James Park");
    strcpy(destination_received,"St. James Park");
}
static void source101(GtkWidget* button2, GtkWidget* widget)
{
    gtk_button_set_label(GTK_BUTTON(button2),"  Holborn");
    strcpy(destination_received,"Holborn");
}

static void button_print_clicked(GtkWidget** labels, GtkWidget* widget)
{
    GdkColor color,color1;
    int loop1,loop2;
    gdk_color_parse ("#FF0000", &color);
    gdk_color_parse ("#0000FF", &color1);
    //gtk_widget_modify_fg (labels[0], GTK_STATE_NORMAL, &color);
    //gtk_label_set_text(GTK_LABEL(labels[0]),str[1]);
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<=final_display_counter;j++)
        {
            if(strcmp(gtk_label_get_text(GTK_LABEL(labels[i])),final_display_result_gtk[j])==0)
                gtk_widget_modify_fg (labels[i], GTK_STATE_NORMAL, &color);
        }
    }
    for(int i=0 ; i<10 ;i++)
    {
        if(strcmp(gtk_label_get_text(GTK_LABEL(labels[i])),mid_point)==0)
                gtk_widget_modify_fg (labels[i], GTK_STATE_NORMAL, &color1);
    }
}

static void button2_clicked(GtkWidget** entry, GtkWidget* widget)
{
}
static void button_clicked1(GtkWidget** windows, GtkWidget* widget)
{
    gtk_widget_hide(windows[0]);


    create_graph();
    display_graphs();
    char c;
    printf("\n\n%s %s\n\n",source_received,destination_received);
    int source_address = find_address(source_received);
    int destination_address = find_address(destination_received);
    int reached_flag = 0;
    int flag_print = 0;

    strcpy(tempo1[temp1_k], source_received);
    temp1_k++;

    strcpy(tempo2[temp2_k], destination_received);
    temp2_k++;
    int counter_source = 0, counter_destination = 0;

    while (!reached_flag)
    {
        cout << "Bidirectional from source node direction are "<<endl;
        if (source_count == 0)
        {
            visited_source[source_count] = london_underground[source_address];
            source_count++;
        }
        int temp = source_count;
        for (int i = counter_source; i < temp; i++)
        {
            bfs_source(visited_source[i]);
            counter_source++;
        }
       
       
        for (int i = temp; i < source_count; i++)
        {
            if (check_temp1(visited_source[i].station_name))
            {
                cout << visited_source[i].station_name << endl;
                strcpy(tempo1[temp1_k], visited_source[i].station_name);
                temp1_k++;
            }
        }
        cout << "Bidirectional from destination node direction are " << endl;
       
       
       
        if (destination_count == 0)
        {
            visited_destination[destination_count] = london_underground[destination_address];
            destination_count++;
        }

        temp = destination_count;
        for (int i = counter_destination; i < temp; i++)
        {
            bfs_destination(visited_destination[i]);
        }
        for (int i = temp; i < destination_count; i++)
        {
            if (check_temp2(visited_destination[i].station_name))
            {
                cout << visited_destination[i].station_name << endl;
                strcpy(tempo2[temp2_k], visited_destination[i].station_name);
                temp2_k++;
            }
        }
        for (int i = source_count-1; i >=0 ; i--)
        {
            for (int j = destination_count-1; j >=0 ; j--)
            {
                if (strcmp(visited_source[i].station_name, visited_destination[j].station_name) == 0)
                {
                    cout << "Here we meet at " << visited_destination[j].station_name;
                    reached_flag = 1;
                    cout << "Path printing time now " << endl;
                    display_path_source(visited_source[i], visited_destination[j].station_name);
                    display_path_destination(visited_destination[j]);
                    final_gtk_display();
                    flag_print = 1;
                    break;
                }
            }
            if(flag_print==1)
                break;
        }
        if(flag_print==1)
            break;
    }
   
    gtk_widget_show_all(windows[1]);

    //printf("\nI want %d\n",top);
    //for(int i=0;i<top;i++)
    //    printf("%s ",str[i]);
    //printf("Done");
}

int show_popup(GtkWidget *widget, GdkEvent *event) {
 
  const gint RIGHT_CLICK = 3;
   
  GdkEventButton *bevent = (GdkEventButton *) event;  
  gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL,bevent->button, bevent->time);
         
  return TRUE;
}




int main(int argc,char* argv[])
{

    PangoFontDescription *df;

    df = pango_font_description_from_string("Monospace");

    pango_font_description_set_size(df,20*PANGO_SCALE);
    
    static GtkWidget *windows[2];
    GtkWidget *pmenu,*pmenu1;
      GtkWidget *s1,*s2,*s3,*s4,*s5,*s6,*s7,*s8,*s9,*s10;
      GtkWidget *s11,*s21,*s31,*s41,*s51,*s61,*s71,*s81,*s91,*s101;
    gtk_init(&argc,&argv);
    GtkWidget *window,*window1,*label1,*label2,*entry1,*entry2,*button,*table,*button2,*entry3,*button3,*button4;
    window1=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
    windows[0] = window;
    windows[1] = window1;

    table=gtk_table_new(10,10,0);
    label1=gtk_label_new("Hello tHere");
    entry1=gtk_entry_new();
    gtk_label_set_text(GTK_LABEL(label1),"Enter Destination");
    gtk_table_attach(GTK_TABLE(table),label1,0,1,4,5,GTK_FILL,GTK_FILL,0,0);
   


    pmenu = gtk_menu_new();
      s1 = gtk_menu_item_new_with_label("Oxford Circus");
      gtk_widget_show(s1);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s1);
      s2 = gtk_menu_item_new_with_label("Bond Street");
      gtk_widget_show(s2);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s2);
      s3 = gtk_menu_item_new_with_label("Marble Arch");
      gtk_widget_show(s3);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu),s3);
      s4 = gtk_menu_item_new_with_label("Lancester Gate");
      gtk_widget_show(s4);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s4);
      s5 = gtk_menu_item_new_with_label("Tottenham Court Road");
      gtk_widget_show(s5);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s5);
      s6 = gtk_menu_item_new_with_label("Warren Street");
      gtk_widget_show(s6);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s6);
      s7 = gtk_menu_item_new_with_label("Green Park");
      gtk_widget_show(s7);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s7);
      s8 = gtk_menu_item_new_with_label("Euston");
      gtk_widget_show(s8);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s8);
      s9 = gtk_menu_item_new_with_label("St. James Park");
      gtk_widget_show(s9);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s9);
      s10 = gtk_menu_item_new_with_label("Holborn");
      gtk_widget_show(s10);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), s10);

    pmenu1 = gtk_menu_new();
      s11 = gtk_menu_item_new_with_label("Oxford Circus");
      gtk_widget_show(s11);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s11);
      s21 = gtk_menu_item_new_with_label("Bond Street");
      gtk_widget_show(s21);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s21);
      s31 = gtk_menu_item_new_with_label("Marble Arch");
      gtk_widget_show(s31);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1),s31);
      s41 = gtk_menu_item_new_with_label("Lancester Gate");
      gtk_widget_show(s41);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s41);
      s51 = gtk_menu_item_new_with_label("Tottenham Court Road");
      gtk_widget_show(s51);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s51);
      s61 = gtk_menu_item_new_with_label("Warren Street");
      gtk_widget_show(s61);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s61);
      s71 = gtk_menu_item_new_with_label("Green Park");
      gtk_widget_show(s71);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s71);
      s81 = gtk_menu_item_new_with_label("Euston");
      gtk_widget_show(s81);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s81);
      s91 = gtk_menu_item_new_with_label("St. James Park");
      gtk_widget_show(s91);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s91);
      s101 = gtk_menu_item_new_with_label("Holborn");
      gtk_widget_show(s101);
      gtk_menu_shell_append(GTK_MENU_SHELL(pmenu1), s101);

    label2=gtk_label_new("Enter Source");
    entry2=gtk_entry_new();
    gtk_table_attach(GTK_TABLE(table),label2,0,1,3,4,GTK_FILL,GTK_FILL,0,0);
    button=gtk_button_new_with_mnemonic("Enter Destination");
    entry3=gtk_entry_new();
    button2=gtk_button_new_with_mnemonic("  --Select--");
    gtk_table_attach(GTK_TABLE(table),button2,2,3,3,4,GTK_FILL,GTK_FILL,0,0);
    button3=gtk_button_new_with_mnemonic("  --  Select  --");
    gtk_table_attach(GTK_TABLE(table),button3,2,3,4,5,GTK_FILL,GTK_FILL,0,0);
    button4=gtk_button_new_with_mnemonic("Search");
    gtk_table_attach(GTK_TABLE(table),button4,1,2,5,6,GTK_FILL,GTK_FILL,0,0);
    //gtk_table_attach(GTK_TABLE(table),button,0,1,4,5,GTK_FILL,GTK_FILL,0,0);

    gtk_signal_connect_object (GTK_OBJECT (s1), "activate",G_CALLBACK(source1),button2);
    gtk_signal_connect_object (GTK_OBJECT (s2), "activate",G_CALLBACK(source2),button2);
    gtk_signal_connect_object (GTK_OBJECT (s3), "activate",G_CALLBACK(source3),button2);
    gtk_signal_connect_object (GTK_OBJECT (s4), "activate",G_CALLBACK(source4),button2);
    gtk_signal_connect_object (GTK_OBJECT (s5), "activate",G_CALLBACK(source5),button2);
    gtk_signal_connect_object (GTK_OBJECT (s6), "activate",G_CALLBACK(source6),button2);
    gtk_signal_connect_object (GTK_OBJECT (s7), "activate",G_CALLBACK(source7),button2);
    gtk_signal_connect_object (GTK_OBJECT (s8), "activate",G_CALLBACK(source8),button2);
    gtk_signal_connect_object (GTK_OBJECT (s9), "activate",G_CALLBACK(source9),button2);
    gtk_signal_connect_object (GTK_OBJECT (s10), "activate",G_CALLBACK(source10),button2);

    gtk_signal_connect_object (GTK_OBJECT (s11), "activate",G_CALLBACK(source11),button3);
    gtk_signal_connect_object (GTK_OBJECT (s21), "activate",G_CALLBACK(source21),button3);
    gtk_signal_connect_object (GTK_OBJECT (s31), "activate",G_CALLBACK(source31),button3);
    gtk_signal_connect_object (GTK_OBJECT (s41), "activate",G_CALLBACK(source41),button3);
    gtk_signal_connect_object (GTK_OBJECT (s51), "activate",G_CALLBACK(source51),button3);
    gtk_signal_connect_object (GTK_OBJECT (s61), "activate",G_CALLBACK(source61),button3);
    gtk_signal_connect_object (GTK_OBJECT (s71), "activate",G_CALLBACK(source71),button3);
    gtk_signal_connect_object (GTK_OBJECT (s81), "activate",G_CALLBACK(source81),button3);
    gtk_signal_connect_object (GTK_OBJECT (s91), "activate",G_CALLBACK(source91),button3);
    gtk_signal_connect_object (GTK_OBJECT (s101), "activate",G_CALLBACK(source101),button3);

    g_signal_connect_swapped(button,"clicked",G_CALLBACK(button_clicked),NULL);

    g_signal_connect_swapped(button2,"clicked",G_CALLBACK(show_popup),pmenu);
    g_signal_connect_swapped(button3,"clicked",G_CALLBACK(show_popup),pmenu1);
    g_signal_connect_swapped(button4,"clicked",G_CALLBACK(button_clicked1),windows);
    gtk_table_set_row_spacings (GTK_TABLE(table),25);
    gtk_table_set_col_spacings (GTK_TABLE(table),25);

    //gtk_container_add(GTK_CONTAINER(window),label);
    gtk_container_add(GTK_CONTAINER(window),table);
    gtk_widget_set_size_request(window,500,300);
    gtk_window_set_title(GTK_WINDOW(window),"Bidirectional Search");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);



//    gtk_widget_show_all(window);


    static GtkWidget *label[10];
    GtkWidget *layout;
    GtkWidget *image;
    GtkWidget *dare,*button_print,*Label1,*Label2,*Label3,*Label4,*Label5,*Label6,*Label7,*Label8,*Label9,*Label10,*Label_u1,*Label_u2,*Label_u3,*Label_u4,*Label_u5,*Label_u6,*Label_u7;
   
    g_signal_connect(window1,"delete-event",G_CALLBACK(gtk_main_quit),NULL);

    //table=gtk_table_new(10,10,0);
    //darea = gtk_drawing_area_new();
      //gtk_container_add(GTK_CONTAINER(window1), darea);
    //gtk_table_attach(GTK_TABLE(table),darea,0,5,0,5,GTK_FILL,GTK_FILL,0,0);
   
    layout = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER (window1), layout);
    gtk_widget_show(layout);

    image = gtk_image_new_from_file("back.jpg");
    gtk_layout_put(GTK_LAYOUT(layout), image, 0, 0);

    button_print = gtk_button_new_with_mnemonic("Show nodes");

    Label1=gtk_label_new("Oxford Circus");
    Label2=gtk_label_new("Bond Street");
    Label3=gtk_label_new("Marble Arch");
    Label4=gtk_label_new("Lancester Gate");
    Label5=gtk_label_new("Tottenham Court Road");
    Label6=gtk_label_new("Warren Street");
    Label7=gtk_label_new("Green Park");
    Label8=gtk_label_new("Euston");
    Label9=gtk_label_new("St. James Park");
    Label10=gtk_label_new("Holborn");
    Label_u1=gtk_label_new("Unused1");
    Label_u2=gtk_label_new("Unused2");
    Label_u3=gtk_label_new("Unused3");
    Label_u4=gtk_label_new("Unused4");
    Label_u5=gtk_label_new("Unused5");
    Label_u6=gtk_label_new("Unused6");
    Label_u7=gtk_label_new("Unused7");
   
    gtk_label_set_text(GTK_LABEL(Label1),"Oxford Circus");
    gtk_label_set_text(GTK_LABEL(Label2),"Bond Street");
    gtk_label_set_text(GTK_LABEL(Label3),"Marble Arch");
    gtk_label_set_text(GTK_LABEL(Label4),"Lancester Gate");
    gtk_label_set_text(GTK_LABEL(Label5),"Tottenham Court Road");
    gtk_label_set_text(GTK_LABEL(Label6),"Warren Street");
    gtk_label_set_text(GTK_LABEL(Label7),"Green Park");
    gtk_label_set_text(GTK_LABEL(Label8),"Euston");
    gtk_label_set_text(GTK_LABEL(Label9),"St. James Park");
    gtk_label_set_text(GTK_LABEL(Label10),"Holborn");
    gtk_label_set_text(GTK_LABEL(Label_u1),"          ");
    gtk_label_set_text(GTK_LABEL(Label_u2),"  ");
    gtk_label_set_text(GTK_LABEL(Label_u3),"");
    gtk_label_set_text(GTK_LABEL(Label_u4),"                      ");

    label[0]=Label1;
    label[1]=Label2;
    label[2]=Label3;
    label[3]=Label4;
    label[4]=Label5;
    label[5]=Label6;
    label[6]=Label7;
    label[7]=Label8;
    label[8]=Label9;
    label[9]=Label10;

   
     //g_signal_connect(G_OBJECT(darea), "expose_event",G_CALLBACK(draw_callback), NULL);

    /*gtk_table_attach(GTK_TABLE(table),label_u1,0,1,0,1,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label1,3,5,1,2,GTK_FILL,GTK_FILL,0,0);

    //gtk_table_attach(GTK_TABLE(table),label_u2,0,1,2,3,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label2,0,1,3,4,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label_u2,1,2,3,4,GTK_FILL,GTK_FILL,0,0); 
    gtk_table_attach(GTK_TABLE(table),label6,2,3,3,4,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label_u3,3,4,3,4,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label5,4,5,3,4,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label_u4,6,7,3,4,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label7,7,8,3,4,GTK_FILL,GTK_FILL,0,0);

    gtk_table_attach(GTK_TABLE(table),label3,1,2,5,6,GTK_FILL,GTK_FILL,0,0); 
    gtk_table_attach(GTK_TABLE(table),label8,3,4,5,6,GTK_FILL,GTK_FILL,0,0);
    gtk_table_attach(GTK_TABLE(table),label9,6,7,5,6,GTK_FILL,GTK_FILL,0,0);

    gtk_table_attach(GTK_TABLE(table),label4,2,3,7,8,GTK_FILL,GTK_FILL,0,0);


    gtk_table_attach(GTK_TABLE(table),button,2,3,9,10,GTK_FILL,GTK_FILL,0,0);



    gtk_table_set_row_spacings (GTK_TABLE(table),40);
    gtk_table_set_col_spacings (GTK_TABLE(table),5);

    //gtk_container_add(GTK_CONTAINER(window),label);
*/
    gtk_layout_put(GTK_LAYOUT(layout), Label1, 230*1.8 + 10, 45*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label2, 20*1.8, 150*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label6, 190*1.8, 148*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label7, 350*1.8, 145*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label5, 500*1.8 + 20, 145*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label3, 40*1.8, 275*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label8, 175*1.8 + 20, 255*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label9, 310*1.8 -15 , 255*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label10, 465*1.8, 255*1.8);
    gtk_layout_put(GTK_LAYOUT(layout), Label4, 185*1.8 + 10, 390*1.8 + 15);
    gtk_layout_put(GTK_LAYOUT(layout), button_print, 500*1.8, 450*1.8 + 10);
    //gtk_container_add(GTK_CONTAINER(window1),table);
	gtk_widget_modify_font(Label1, df);
	gtk_widget_modify_font(Label10, df);
	gtk_widget_modify_font(Label9, df);
	gtk_widget_modify_font(Label8, df);
	gtk_widget_modify_font(Label7, df);
	gtk_widget_modify_font(Label6, df);
	gtk_widget_modify_font(Label5, df);
	gtk_widget_modify_font(Label4, df);
	gtk_widget_modify_font(Label3, df);
	gtk_widget_modify_font(Label2, df);
	gtk_widget_modify_font(Label1, df); 
    g_signal_connect_swapped(button_print,"clicked",G_CALLBACK(button_print_clicked),label);

    gtk_widget_set_size_request(window1,640*1.8,500*1.8);
    gtk_window_set_title(GTK_WINDOW(window1),"Bidirectional Search");
    gtk_window_set_position(GTK_WINDOW(window1), GTK_WIN_POS_CENTER);


   


    gtk_widget_show_all(window);
    gtk_main();

}
