# Implementação

Devido a uma definição incorreta (e até caotica) da estrutura da solução, uma única classe (anteriormente chamada WallsRender) desempenhava simultaneamente as funções de Render e Walls, enquanto a função Character era responsável por parte das funções de Render. Emfim toda antiga estrutura foi descartada 

Optou-se por utilizar classes da seção QGraphics do framework Qt, tais como QGraphicsScene e QGraphicsItem, em vez de criar classes próprias do zero. Isso simplificou um pouco a integração com o Qt e permitiu o uso de funções prontas, como QGraphicsItem::collidesWithItem()



<div align="center">

[Retroceder](projeto.md) | [Avançar](testes.md)

</div>