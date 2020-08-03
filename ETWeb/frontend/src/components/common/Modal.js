import React, { useState } from 'react';
import { Button, Modal, ModalHeader, ModalBody, ModalFooter } from 'reactstrap';

export default props => {
  const {
    modalTitle,
    triggerBtnLabel,
    actionBtnLabel,
    className,
    onAction,
    modal,
    onToggle
  } = props;

  // const [modal, setModal] = useState(false);

  // const toggle = () => setModal(!modal);

  return (
      <div>
          <div className={className}>
            <Button color="secondary"
                    onClick={onToggle}
                    className="mx-2"
            >
              {triggerBtnLabel}
            </Button>
          </div>
      <Modal isOpen={modal} toggle={onToggle} className={className}>
          <ModalHeader toggle={onToggle}>{modalTitle}</ModalHeader>
          <ModalBody>
            {props.children}
          </ModalBody>
          <ModalFooter>
            <Button color="success" onClick={onAction}>{actionBtnLabel}</Button>{' '}
            <Button color="secondary" onClick={onToggle}>Cancel</Button>
          </ModalFooter>
        </Modal>
      </div>
  );
};