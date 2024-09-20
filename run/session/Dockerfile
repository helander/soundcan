FROM sqimage-jalv-http AS jalv
FROM sqimage-lv2soundfont AS lv2soundfont
FROM sqimage-setbfree AS setbfree
FROM sqimage-setbfreecontrol AS setbfreecontrol
FROM sqimage-modhost AS modhost

FROM sqimage-run-platform

COPY --from=jalv / /
COPY --from=lv2soundfont / /
COPY --from=setbfree / /
COPY --from=setbfreecontrol / /

COPY --from=modhost / /
RUN apt install libreadline-dev

ADD entrypoint.sh /

ENTRYPOINT ["/entrypoint.sh"]
