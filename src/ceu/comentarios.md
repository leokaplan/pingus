1. identificar estado
2. qual o evento associado (que muda o estado)
3. quando a classe que contem o estado nao for unica (varias instancias para um mesmo evento), criar uma associacao entre a classe C e a Ceu (passando a referencia de this)
4. refazer o controle da maquina em Ceu e remover de C
5. hook de C para Ceu 
    chamada do evento ceu (ceu_sys_evt_go)
extra 


    MenuButton::MenuButton(PingusMenu* menu_,...){
        ...

            ceu_sys_event(CEU_IN_NEWMENU, &this);
    }//passo 5

GroupComponent::on_pointer_move(int x, int y){
    ...
        if (comp != mouse_over_comp)
        {
            if (mouse_over_comp)
            {
            mouse_over_comp->set_mouse_over(false);
            mouse_over_comp->on_pointer_leave();
          }
          
          if (comp)
          {
            comp->set_mouse_over(true);
            comp->on_pointer_enter();
            ceu_sys_event(CEU_IN_ON_POINTER_ENTER, &comp);//passo 3
          }
}
class MenuButton with//passo 3
var _MenuButton* me;
do
    loop do//passo 3
        var _GUIElemnt* gui = await ON_POINTER_ENTER
                              until (gui == me);
        mouse_over = true;
        var _GUIElemnt* gui = await ON_POINTER_ENTER
                              until (gui == me);
        mouse_over = false;
    end
end

var _MenuButton* but;
every but in NEWBUTTON do//passo 3
    spawn MenuButton with
        this.me = but;
    end;
end

void
MenuButton::on_pointer_enter () //passo 2
{
  mouse_over = true; //passo 1
  Sound::PingusSound::play_sound ("tick");
  menu->set_hint(desc);
}

void
MenuButton::on_pointer_leave () //passo 2
{
  mouse_over = false; //passo 1
  menu->set_hint("");
}
