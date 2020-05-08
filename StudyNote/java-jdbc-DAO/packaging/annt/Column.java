package com.packaging.annt;

import java.lang.annotation.*;

@Documented
@Target(ElementType.FIELD)
@Retention(RetentionPolicy.RUNTIME)
public @interface Column {
    public String value() default "";
    public boolean select() default true;
    public String condition() default "=";
}

