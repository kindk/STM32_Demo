This is a key readme.
To use this key module, follow below 5 steps:

1. Define your key and global fifo(used for storing key events)
    
    key_t key;
    fifo_t key_fifo;
2. Call key_init(key_t *key, u8 id, GPIO_TypeDef *gpio_group, u16 pin, u8 active, u16 jitter_ms, fifo_t *fifo)
    id: is the unique id number for this key, it will bind to events
    active: indicates whether gpio high or low when key is pressed. if active=0, means when key pressed, the gpio is low
    jitter_ms: if zero, will use default jitter time: 100ms
    fifo: the global fifo for storing key events
    
    key_init(&key,0, GPIOB, GPIO_Pin_9, 0, 100, &key_fifo);
3. Define your key's exti irqhandler
    For pin 0-4: EXTI0_IRQHandler, EXTI1_IRQHandler
    For pin 5-9: EXTI9_5_IRQHandler
    for pin 10-15: EXTI15_10_IRQHandler
    
    void EXTI9_5_IRQHandler()
    {
        key_irq_handler(&key);
    }
3. Call key_state_machine(key_t *key) every 10ms
    key_state_machine(&key);
4. Define your key process as below, and called every 10ms:
    void key_process(key_t *key)
    {
        key_event_t event;
        u8 len;
        if (!key->fifo->is_empty(key->fifo)) {
            key->fifo->out(key->fifo, (u8 *)&event, &len);
            LOGI("code: %d, action: %s", event.id, key_action_string[event.action]);
        }
    }
